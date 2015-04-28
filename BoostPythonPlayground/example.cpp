#include <iostream>
#include <memory>
#include <boost/python.hpp>

/// @brief Mockup Spam class.
struct Spam
{
	Spam(std::size_t x) : x(x) { std::cout << "Spam()" << std::endl; }
	~Spam() { std::cout << "~Spam()" << std::endl; }
	Spam(const Spam&) = delete;
	Spam& operator=(const Spam&) = delete;
	std::size_t x;
};

/// @brief Mockup factor for Spam.
struct SpamFactory
{
	/// @brief Create Spam instances.
	std::unique_ptr<Spam> make(const std::string& str)
	{
		return std::unique_ptr<Spam>{new Spam{ str.size() }};
	}

	/// @brief Delete Spam instances.
	void consume(std::unique_ptr<Spam>) {}
};

/// @brief Adapter a non-member function that returns a unique_ptr to
///        a python function object that returns a raw pointer but
///        explicitly passes ownership to Python.
template <typename T,
	typename ...Args>
	boost::python::object adapt_unique(std::unique_ptr<T>(*fn)(Args...))
{
	return boost::python::make_function(
		[fn](Args... args) { return fn(args...).release(); },
		boost::python::return_value_policy<boost::python::manage_new_object>(),
		boost::mpl::vector<T*, Args...>()
		);
}

/// @brief Adapter a member function that returns a unique_ptr to
///        a python function object that returns a raw pointer but
///        explicitly passes ownership to Python.
template <typename T,
	typename C,
	typename ...Args>
	boost::python::object adapt_unique(std::unique_ptr<T>(C::*fn)(Args...))
{
	return boost::python::make_function(
		[fn](C& self, Args... args) { return (self.*fn)(args...).release(); },
		boost::python::return_value_policy<boost::python::manage_new_object>(),
		boost::mpl::vector<T*, C&, Args...>()
		);
}

/// @brief Wrapper function for SpamFactory::consume().  This
///        is required because Boost.Python will pass a handle to the
///        Spam instance as an auto_ptr that needs to be converted to
///        convert to a unique_ptr.
void SpamFactory_consume(
	SpamFactory& self,
	std::auto_ptr<Spam> ptr) // Note auto_ptr provided by Boost.Python.
{
	return self.consume(std::unique_ptr<Spam>{ptr.release()});
}

BOOST_PYTHON_MODULE(example)
{
	namespace python = boost::python;
	python::class_<Spam, boost::noncopyable>(
		"Spam", python::init<std::size_t>())
		.def_readwrite("x", &Spam::x)
		;

	python::class_<SpamFactory>("SpamFactory", python::init<>())
		.def("make", adapt_unique(&SpamFactory::make))
		.def("consume", &SpamFactory_consume)
		;
}