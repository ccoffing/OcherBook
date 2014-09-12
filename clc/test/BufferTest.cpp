#include "clc/data/Buffer.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"


TEST_CASE("Buffer constructors")
{
	char const* source = "Hello World!";

	SECTION("Trivial")
	{
		clc::Buffer a;
		CHECK(a.length() == 0U);
	}

	SECTION("C string")
	{
		clc::Buffer a(source);
		CHECK(strlen(source) == (size_t)a.length());
		CHECK(a.c_str() != source);  // Should have copied
		CHECK(a == source);
	}

	SECTION("Truncated C string")
	{
		clc::Buffer a(source, 5);
		CHECK(5U == a.length());
		CHECK(a.c_str() != source);  // Should have copied
		CHECK(strncmp(a.c_str(), source, 5) == 0);
	}
}

TEST_CASE("Buffer utf8")
{
	// 4 characters:
	//   ampersand     1 byte
	//   cyrillic zhe  2 bytes
	//   chinese       3 bytes
	//   symbol        4 bytes
	clc::Buffer a("&\xd0\x96\xe4\xb8\xad\xf0\x90\x8d\x86");
	CHECK(10U == a.length());
	CHECK(4U == a.countChars());
}

TEST_CASE("Buffer format")
{
	clc::Buffer a;
	a.format("Big number: %x", 0x12345678);
	CHECK(20U == a.length());
}

TEST_CASE("Buffer appendFormat")
{
	clc::Buffer a;
	a = "Big number";
	a.appendFormat(": %x", 0x12345678);
	CHECK(20U == a.length());
}

TEST_CASE("Buffer copyOnWrite")
{
	clc::Buffer a("Hello World");
	clc::Buffer b(a);
	b.append(" Domination");
	CHECK(a == clc::Buffer("Hello World"));
	CHECK(b == clc::Buffer("Hello World Domination"));
}

TEST_CASE("Buffer c_str")
{
	const char a[] = "multi\nline\nstring\n";
	clc::Buffer b(a);
	CHECK(memcmp(b.c_str(), a, sizeof(a)) == 0);
}

TEST_CASE("Buffer copy")
{
	clc::Buffer* s2;
	clc::Buffer* s4;
	clc::Buffer s5;
	{
		clc::Buffer s1("test123");         // Constructor, on stack
		s2 = new clc::Buffer("test456");   // Constructor, on heap
		clc::Buffer  s3(s1);               // Copy constructor, on stack
		s4 = new clc::Buffer(s3);          // Copy constructor, on heap
		s5 = s3;                          // Assignment, to stack value which is going away
	}
	delete s2;
	delete s4;
	// Should not crash when s5 gets destructed (was shared with now-defunct s3)
	// Should have no leaks
}

TEST_CASE("Buffer binary")
{
	const size_t len = 16;
	const char data[len] = {
		0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x18,
		0x19, 0x00, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
	};
	clc::Buffer s1(data, len);
	CHECK(s1.length() == len);
	CHECK(memcmp(s1.c_str(), data, len) == 0);
	// Copies should be exact -- not somehow coerced to a C string
	clc::Buffer s2(s1);
	CHECK(s2.length() == len);
	CHECK(memcmp(s2.c_str(), data, len) == 0);
}

TEST_CASE("Buffer compare")
{
	clc::Buffer s1("testing");
	clc::Buffer s2("testing");
	clc::Buffer s3("Testing");
	clc::Buffer s4("tester");
	clc::Buffer s5("test");

	CHECK(s1 == s2);
	CHECK(s1 != s3);
	CHECK(s1.Compare(s2) == 0);
	CHECK(s1.Compare(s2.c_str()) == 0);
	CHECK(s1.Compare(s3) != 0);
	CHECK(s1.ICompare(s2) == 0);
	CHECK(s1.ICompare(s3) == 0);
	CHECK(s1 > s5);
	CHECK(s1.Compare(s5) > 0);
	CHECK(s1.Compare(s5.c_str()) > 0);
	CHECK(s5 < s1);
	CHECK(s5.Compare(s1) < 0);
	CHECK(s5.Compare(s1.c_str()) < 0);
	CHECK(s1.Compare(s4, 4) == 0);
	CHECK(s3.ICompare(s4, 4) == 0);
	CHECK(s1.Compare(s4, 5) != 0);
	CHECK(s3.ICompare(s4, 5) != 0);
}

TEST_CASE("Buffer binaryCompare")
{
	clc::Buffer s1("te\0ting", 7);
	clc::Buffer s2("te\0ting", 7);
	clc::Buffer s3("Te\0ting", 7);
	clc::Buffer s4("te\0ter", 6);
	clc::Buffer s5("te\0t", 4);

	CHECK(s1 == s2);
	CHECK(s1 != s3);
	CHECK(s1.Compare(s2) == 0);
	CHECK(s1.Compare(s2.c_str()) > 0);
	CHECK(s1.Compare(s3) != 0);
	CHECK(s1.ICompare(s2) == 0);
	CHECK(s1.ICompare(s3) == 0);
	CHECK(s1 > s5);
	CHECK(s1.Compare(s5) > 0);
	CHECK(s1.Compare(s5.c_str()) > 0);
	CHECK(s5 < s1);
	CHECK(s5.Compare(s1) < 0);
	CHECK(s5.Compare(s1.c_str()) > 0);
	CHECK(s1.Compare(s4, 4) == 0);
	CHECK(s3.ICompare(s4, 4) == 0);
	CHECK(s1.Compare(s4, 5) != 0);
	CHECK(s3.ICompare(s4, 5) == 0);  // Case implies all treated as C string
}

TEST_CASE("Buffer setToTrunc")
{
	clc::Buffer s;
	s.setTo("testing");
	CHECK(s == clc::Buffer("testing"));
	s.setTo(s, 4);
	CHECK(s == clc::Buffer("test"));
}
