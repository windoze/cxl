import clang.cindex, asciitree, sys, md5
from mako.template import Template
from argparse import ArgumentParser
parser = ArgumentParser()
parser.add_argument("-I", dest="inc", action="append")
parser.add_argument("-D", dest="macro", action="append")
parser.add_argument("-o", "--output", dest="output", default="-")
(options, args) = parser.parse_known_args()
files=[a for a in args if a[0]!='-']
if (len(args)==0):
	parser.error("no input files");
	sys.exit()

kept=['-x', 'c++', '-std=c++14', '-DCXL_REFLECTED_CODE_GENERATOR']
if options.inc!=None:
	kept.extend(["-I"+i for i in options.inc])
if options.macro!=None:
	kept.extend(["-D"+i for i in options.macro])

clang.cindex.Config.set_library_path("/Library/Developer/CommandLineTools/usr/lib")
index = clang.cindex.Index(clang.cindex.conf.lib.clang_createIndex(False, True))

tmpl=r"""///////////////////////////////////////////////////////////////////////////
// GENERATED CODE
///////////////////////////////////////////////////////////////////////////

#ifndef CXL_REFLECTED_CODE_GENERATOR

#ifndef ${header_guard}
#define ${header_guard}

#include <cxutil/reflection/reflection_impl.hpp>

% for c in classes:
#ifndef ${c.header_guard()}
#define ${c.header_guard()}

namespace cxutil { namespace reflection {
% for m in enumerate(c.members):
template<>
struct reflected_element<${m[0]}, ${c.identifier()}> {
	static constexpr char const *name() { return "${m[1].name}"; };
	static constexpr char const *key() { return "${m[1].serialization_key}"; };
	typedef decltype(${c.identifier()}::${m[1].name}) type;
	static constexpr type &get(${c.identifier()} &d) { return d.${m[1].name}; }
	static constexpr type &&get(${c.identifier()} &&d) { return std::move(d.${m[1].name}); }
	static constexpr type const &get(${c.identifier()} const &d) { return d.${m[1].name}; }
};
template<>
struct reflected_element<${m[0]}, const ${c.identifier()}> : public reflected_element<${m[0]}, ${c.identifier()}> {
	typedef std::add_const<decltype(${c.identifier()}::${m[1].name})>::type type;
};
% endfor

template<>
struct reflected<${c.identifier()}> {
	static constexpr bool enabled=true;
	typedef std::tuple<
		${c.member_identifier_list(prefix="decltype(", postfix=")", sep=",\n\t\t")}
	> tuple_type;
	typedef cxutil::reflection::to_variant_t<tuple_type> variant_type;

	static constexpr std::size_t attr_count=${len(c.members)};

	static constexpr std::size_t size() { return attr_count; }
	static constexpr char const *name() { return "${c.name}"; }
	static constexpr char const *full_name() { return "${c.identifier()}"; }

	static const std::string &get_annotation(const std::string &key) {
		static const std::map<std::string, std::string> annotations {
			% for ann in c.annotations:
			{"${ann}", "${c.annotations[ann]}"},
			% endfor
		};
		return annotations.at(key);
	}

	static const std::string &get_attr_annotation(const std::string &name, const std::string &key) {
		static const std::map<std::string, std::map<std::string, std::string>> attr_annotations {
			% for m in c.members:
			{"${m.name}", {
			% for ann in m.annotations:
				{"${ann}", "${m.annotations[ann]}"},
			% endfor
			}},
			% endfor
		};
		return attr_annotations.at(name).at(key);
	}

	static void *get_attr_ptr(${c.identifier()} &d, std::size_t N) {
		switch(N) {
		% for m in enumerate(c.members):
			% if not m[1].is_const:
		case ${m[0]}: return &(d.${m[1].name});
			% endif
		% endfor
		}
		throw std::out_of_range("reflected<${c.identifier()}>::get_attr_ptr");
	}

	static const void *get_attr_ptr(const ${c.identifier()} &d, std::size_t N) {
		switch(N) {
		% for m in enumerate(c.members):
			% if not m[1].is_const:
		case ${m[0]}: return &(d.${m[1].name});
			% endif
		% endfor
		}
		throw std::out_of_range("reflected<${c.identifier()}>::get_attr_ptr");
	}

	decltype(auto) static make_tuple(const ${c.identifier()} &d) {
		return tuple_type(
			${c.member_name_list(prefix="d.", sep=",\n\t\t\t")}
		);
	}

	decltype(auto) static tie(${c.identifier()} &d) {
		return std::tie(
			${c.member_name_list(prefix="d.", sep=",\n\t\t\t")}
		);
	}
};

template<>
struct reflected<const ${c.identifier()}> : public reflected<${c.identifier()}> {
	typedef std::tuple<
		${c.member_identifier_list(prefix="std::add_const<decltype(", postfix=")>::type", sep=",\n\t\t")}
	> tuple_type;
};

}}	// End of namespace cxutil::reflection

namespace std {
	template<>
	class tuple_size<${c.identifier()}> { public: static constexpr std::size_t value=cxutil::reflection::reflected<${c.identifier()}>::attr_count; };
	template<std::size_t I>
	class tuple_element<I, ${c.identifier()}> : public cxutil::reflection::reflected_element<I, ${c.identifier()}> {};
	template<std::size_t I>
	class tuple_element<I, const ${c.identifier()}> : public cxutil::reflection::reflected_element<I, const ${c.identifier()}> {};
	template<std::size_t I>
	class tuple_element<I, volatile ${c.identifier()}> : public cxutil::reflection::reflected_element<I, volatile ${c.identifier()}> {};
	template<std::size_t I>
	class tuple_element<I, const volatile ${c.identifier()}> : public cxutil::reflection::reflected_element<I, const volatile ${c.identifier()}> {};
}
#endif	// !defined(${c.header_guard()})
% endfor

#endif	// !defined(${header_guard})
#endif	// !defined(CXL_REFLECTED_CODE_GENERATOR)
"""

tpl = Template(text=tmpl)

REFLECT_ANN="CXL_REFLECTABLE"
HIDDEN_ANN="CXL_HIDDEN"
SER_ANN="CXL_SERIALIZATION_KEY"

def get_annotations(node):
	return [c.displayname for c in node.get_children() if c.kind == clang.cindex.CursorKind.ANNOTATE_ATTR]

def parse_annotation(ann, map):
	anns=[p for p in ann.split(",") if len(p.strip())>0]
	for a in anns:
		if('=' in a):
			k,v=a.split("=")
			map[k.strip()]=v.strip()
		else:
			map[a.strip()]=""

def get_annotation_map(node):
	ret={}
	for ann in get_annotations(node):
		parse_annotation(ann, ret)
	return ret

def reflectable(node, fn):
	if node.access_specifier == clang.cindex.AccessSpecifier.PRIVATE:
		return false
	if node.access_specifier == clang.cindex.AccessSpecifier.PROTECTED:
		return false
	if ((node.kind == clang.cindex.CursorKind.CLASS_DECL or node.kind==clang.cindex.CursorKind.STRUCT_DECL)
		and node.location.file.name == fn):
		m=get_annotation_map(node)
		return REFLECT_ANN in m and m[REFLECT_ANN].lower() in ["", "true", "yes"]
	if (node.kind==clang.cindex.CursorKind.FIELD_DECL) and (node.location.file.name == fn):
		m=get_annotation_map(node)
		return HIDDEN_ANN not in m or m[HIDDEN_ANN].lower() in ["false", "no"]
	return False

def conststr(b):
	if(b): return "const "
	return ""

class Member(object):
	def __init__(self, owner, cursor):
		self.owner=owner
		self.name = cursor.spelling
		self.annotations=get_annotation_map(cursor)
		self.is_const=cursor.type.is_const_qualified()
		self.serialization_key=self.annotations.get(SER_ANN, self.name)
	def identifier(self):
		return "%s::%s" % (self.owner.identifier(), self.name)
	def __repr__(self):
		return "%s%s::%s[%s]" % (conststr(self.is_const), self.owner.identifier(), self.name, self.annotations)
	def __str__(self):
		return self.__repr__();

class Class(object):
	def __init__(self, cursor, namespace, fn):
		self.name = cursor.spelling
		self.namespace=namespace
		self.annotations=get_annotation_map(cursor)
		self.members = []
		for c in cursor.get_children():
			if c.access_specifier == clang.cindex.AccessSpecifier.PRIVATE:
				continue
			if c.access_specifier == clang.cindex.AccessSpecifier.PROTECTED:
				continue
			if reflectable(c, fn):
				self.members.append(Member(self, c))
	def identifier(self):
		if len(self.namespace)>0:
			return "::%s::%s" % ("::".join(self.namespace), self.name)
		return "::%s" % self.name
	def __repr__(self):
		return "%s {\n%s\n}" % (self.identifier(), "".join(["\t"+m.__repr__()+";\n" for m in self.members]))
	def __str__(self):
		return self.__repr__();
	def member_name_list(self, prefix="", postfix="", sep=","):
		return sep.join(['%s%s%s'%(prefix, m.name, postfix) for m in self.members])
	def member_identifier_list(self, prefix="", postfix="", sep=","):
		return sep.join(['%s%s%s'%(prefix, m.identifier(), postfix) for m in self.members])
	def header_guard(self):
		return "CXL_REFLECTED"+self.identifier().replace(":", "_")+"_"+md5.md5(self.identifier()).hexdigest().upper()

def build_classes(cursor, fn="", start_ns=[]):
	result = []
	for c in cursor.get_children():
		if c.access_specifier == clang.cindex.AccessSpecifier.PRIVATE:
			continue
		if c.access_specifier == clang.cindex.AccessSpecifier.PROTECTED:
			continue
		if ((c.kind == clang.cindex.CursorKind.CLASS_DECL or c.kind==clang.cindex.CursorKind.STRUCT_DECL)
			and reflectable(c, fn)):
			a_class = Class(c, start_ns, fn)
			result.append(a_class)
			child_classes = build_classes(c, fn, start_ns+[c.spelling])
			result.extend(child_classes)
		elif c.kind == clang.cindex.CursorKind.NAMESPACE:
			child_classes = build_classes(c, fn, start_ns+[c.spelling])
			result.extend(child_classes)
	return result

classes=[]

def process_file(fn):
	translation_unit = index.parse(fn, kept)
	return build_classes(translation_unit.cursor, fn)

for fn in files:
	classes.extend(process_file(fn))
hg="CXL_REFLECTED__"+md5.md5("\n".join([c.identifier() for c in classes])).hexdigest().upper()+"_HPP"
if options.output=='-':
	print tpl.render(classes=classes, header_guard=hg)
else:
	f=open(options.output, "w")
	f.write(tpl.render(classes=classes, header_guard=hg))

