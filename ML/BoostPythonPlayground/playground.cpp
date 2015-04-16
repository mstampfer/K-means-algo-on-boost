#pragma once
#include <list>
#include <vector>
#include <string>
#include <map>
#include <exception>
#include <iterator>

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python.hpp>
#include <boost/python/make_function.hpp>
#include <boost/python/object.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/python/raw_function.hpp>
#include <boost/python/exception_translator.hpp>

using namespace std;
namespace python = boost::python;
typedef multimap<const string, pair<const string, const double>> Critics;

class myClass {
public:
	static char const* foo() { return "foo"; }
	static char const* bar() { return "bar"; }
	static python::object SetParameters(python::tuple args, python::dict kwargs);
	static python::object testDict(python::tuple args, python::dict kwargs);
	static void sequence_to_int_list(const python::object& ob);
	static python::object choose_function(bool selector);
};

python::object myClass::SetParameters(python::tuple args, python::dict kwargs)
{
	python::list keys = kwargs.keys();
	Critics critics;

	for (int i = 0; i < len(keys); ++i)
	{
		string key(python::extract<string>(kwargs.keys()[0]));
		auto curArg = static_cast<python::dict>(kwargs[keys[i]]);
		if (curArg)
		{
			string s = std::move(python::extract<string>(curArg.keys()[0]));
			double  d = std::move(python::extract<double>(curArg.values()[0]));
			critics.insert(make_pair(key, make_pair(s, d)));
		}
	}

	python::tuple vals = python::make_tuple(args[1]);
	python::dict dvals = python::extract<python::dict>(vals[0]);
	keys = dvals.keys();

	for (int i = 0; i < len(keys); ++i)
	{
		auto curArg = static_cast<python::dict>(dvals[keys[i]]);
		if (curArg)
		{
			string key(python::extract<string>(dvals.keys()[i]));
			for (int j = 0; j < len(curArg); ++j)
			{
				string s = move(python::extract<string>(curArg.keys()[j]));
				double  d = move(python::extract<double>(curArg.values()[j]));
				critics.insert(make_pair(key, make_pair(s, d)));
			}
		}
	}

	for (auto& e1 : critics)
		cout << e1.first << " " << e1.second.first << " " << e1.second.second << endl;

	return python::object();
}	

python::object myClass::testDict(python::tuple args, python::dict kwargs)
{
	map<string, int> m;
	if (kwargs)
	{
		python::list l = kwargs.items();
		python::stl_input_iterator<python::tuple> begin(l), end;
		list<python::tuple> li = list<python::tuple>(begin, end);
		std::transform(li.begin(), li.end(), inserter(m, m.end()), [](auto& e)
		{
			string s(move(python::extract<string>(e[0])));
			int n(move(python::extract<int>(e[1])));
			return std::make_pair(s, n);
		});
	}


	if (args)
	{
		multimap<string, pair<string, double>> mm;
		python::tuple vals = python::make_tuple(args[1]);
		python::dict d = python::extract<python::dict>(vals[0]);
		python::list l = d.items();
		python::stl_input_iterator<python::tuple> begin(l), end;
		list<python::tuple> li = list<python::tuple>(begin, end);
		std::transform(li.begin(), li.end(), inserter(mm, mm.end()), [&](auto& e)
		{
			python::dict dct = python::extract<python::dict>(e[1]);
			python::list lst = ((dct).items());
			python::tuple t = python::extract<python::tuple>(lst[0]);
			string key1(move(python::extract<string>(e[0])));
			string key2(move(python::extract<string>(t[0])));
			double d(move(python::extract<double>(t[1])));
			return make_pair(key1, make_pair(key2, d));				   
		});
	}

	return python::object();
}

python::object myClass::choose_function(bool selector)
{
	if (selector)
		return boost::python::make_function(&foo);
	else
		return boost::python::make_function(&bar);
}

void myClass::sequence_to_int_list(const python::object& ob)
{
	python::stl_input_iterator<int> begin(ob), end;
	//list<int> l = list<int>(begin, end);
	vector<int> v = vector<int>(begin, end);
}

char const* foo(int x, int y) { return "foo"; }

class my_def_visitor : python::def_visitor<my_def_visitor>
{
	friend class def_visitor_access;

	template <class classT>
	void visit(classT& c) const
	{
		c
			.def("foo", &my_def_visitor::foo)
			.def("bar", &my_def_visitor::bar)
			;
	}

};

enum color { red = 1, green = 2, blue = 4 };

color identity_(color x) { return x; }

struct my_exception : std::exception
{
	char const* what() const throw() { return "One of my exceptions"; }
};

void translate(my_exception const& e)
{
	// Use the Python 'C' API to set up an exception object
	PyErr_SetString(PyExc_RuntimeError, e.what());
}

void something_which_throws()
{
		throw my_exception();
}


struct X
{
	void f() {}

	struct Y { int g() { return 42; } };
};

BOOST_PYTHON_MODULE(nested)
{
	// add some constants to the current (module) scope
	python::scope().attr("yes") = 1;
	python::scope().attr("no") = 0;

	// Change the current scope 
	python::scope outer
		= python::class_<X>("X")
		.def("f", &X::f)
		;

	// Define a class Y in the current scope, X
	python::class_<X::Y>("Y")
		.def("g", &X::Y::g)
		;
}
BOOST_PYTHON_MODULE(playground)
{
	python::class_<myClass>("myClass")
		.def("SetParameters", python::raw_function(&myClass::SetParameters))
		.def("testDict", python::raw_function(&myClass::testDict))
		.def("sequence_to_int_list", &myClass::sequence_to_int_list)
		.def("choose_function", &myClass::choose_function)
		;
	python::enum_<color>("color")
		.value("red", red)
		.value("green", green)
		.export_values()
		.value("blue", blue)
		;

	python::def("identity", identity_);
	
	python::def("foo", foo, python::args("x", "y"), "foo's docstring");

	python::register_exception_translator<my_exception>(&translate);

	python::def("something_which_throws", something_which_throws);

	// add some constants to the current (module) scope
	python::scope().attr("yes") = 1;
	python::scope().attr("no") = 0;

	// Change the current scope 
	python::scope outer
		= python::class_<X>("X")
		.def("f", &X::f)
		;

	// Define a class Y in the current scope, X
	python::class_<X::Y>("Y")
		.def("g", &X::Y::g)
		;
}