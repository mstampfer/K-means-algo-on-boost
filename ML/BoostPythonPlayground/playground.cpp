#pragma once
#include <list>
#include <string>
#include <map>

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python.hpp>
#include <boost/python/make_function.hpp>
#include <boost/python/object.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/python/raw_function.hpp>

using namespace std;
namespace python = boost::python;
typedef multimap<const string, pair<const string, const double>> Critics;

class myClass {
public:
	static python::object SetParameters(python::tuple args, python::dict kwargs);
	static list<int> sequence_to_int_list(python::object const& ob);		
	static python::object choose_function(bool selector);
	char const* foo() { return "foo"; }
	char const* bar() { return "bar"; }
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

python::object myClass::choose_function(bool selector)
{
	if (selector)
		return boost::python::make_function(&foo);
	else
		return boost::python::make_function(&bar);
}

std::list<int> myClass::sequence_to_int_list(python::object const& ob)
{
	python::stl_input_iterator<int> begin(ob), end;
	return list<int>(begin, end);
}

BOOST_PYTHON_MODULE(playground)
{
	python::class_<myClass>("myClass")
		.def("SetParameters", python::raw_function(&myClass::SetParameters))
		.def("sequence_to_int_list", &myClass::sequence_to_int_list)
		.def("choose_function", &myClass::choose_function)
		;
}