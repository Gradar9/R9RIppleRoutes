//
// Copyright (c) 2013-2016 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// Test that header file is self-contained.
#include <beast/http/rfc7230.hpp>

#include <beast/http/detail/rfc7230.hpp>
#include <beast/unit_test/suite.hpp>
#include <string>
#include <vector>

namespace beast {
namespace http {
namespace test {

class rfc7230_test : public beast::unit_test::suite
{
public:
    static
    std::string
    fmt(std::string const& s)
    {
        return '\'' + s + '\'';
    }

    static
    std::string
    str(boost::string_ref const& s)
    {
        return std::string(s.data(), s.size());
    }

    static
    std::string
    str(param_list const& c)
    {
        std::string s;
        for(auto const& p : c)
        {
            s.push_back(';');
            s.append(str(p.first));
            s.push_back('=');
            s.append(str(p.second));
        }
        return s;
    }

    void
    testParamList()
    {
        auto const ce =
            [&](std::string const& s)
            {
                auto const got = str(param_list{s});
                expect(got == s, fmt(got));
            };
        auto const cs =
            [&](std::string const& s, std::string const& good)
            {
                ce(good);
                auto const got = str(param_list{s});
                ce(got);
                expect(got == good, fmt(got));
            };
        auto const cq =
            [&](std::string const& s, std::string const& good)
            {
                auto const got = str(param_list{s});
                expect(got == good, fmt(got));
            };

        ce("");
        cs(" ;\t i =\t 1 \t", ";i=1");
        cq("\t; \t xyz=1 ; ijk=\"q\\\"t\"", ";xyz=1;ijk=q\"t");

        // invalid strings
        cs(";", "");
        cs(";,", "");
        cs(";xy", "");
        cs(";xy", "");
        cs(";xy ", "");
        cs(";xy,", "");

        cq(";x=,", "");
        cq(";xy=\"", "");
        cq(";xy=\"\x7f", "");
        cq(";xy=\"\\", "");
        cq(";xy=\"\\\x01\"", "");
    }

    static
    std::string
    str(ext_list const& ex)
    {
        std::string s;
        for(auto const& e : ex)
        {
            if(! s.empty())
                s += ',';
            s.append(str(e.first));
            s += str(e.second);
        }
        return s;
    }

    void
    testExtList()
    {
        auto const ce =
            [&](std::string const& s)
            {
                auto const got = str(ext_list{s});
                expect(got == s, fmt(got));
            };
        auto const cs =
            [&](std::string const& s, std::string const& good)
            {
                ce(good);
                auto const got = str(ext_list{s});
                ce(got);
                expect(got == good, fmt(got));
            };
        auto const cq =
            [&](std::string const& s, std::string const& good)
            {
                auto const got = str(ext_list{s});
                expect(got == good, fmt(got));
            };
    /*
        ext-list    = *( "," OWS ) ext *( OWS "," [ OWS ext ] )
        ext         = token param-list
        param-list  = *( OWS ";" OWS param )
        param       = token OWS "=" OWS ( token / quoted-string )
    */
        ce("");
        cs(",", "");
        cs(", ", "");
        cs(",\t", "");
        cs(", \t", "");
        cs(" ", "");
        cs(" ,", "");
        cs("\t,", "");
        cs("\t , \t", "");
        cs(",,", "");
        cs(" , \t,, \t,", "");

        ce("a");
        ce("ab");
        ce("a,b");
        cs(" a ", "a");
        cs("\t a, b\t  ,  c\t", "a,b,c");

        cs("a; \t i\t=\t \t1\t ", "a;i=1");
        ce("a;i=1;j=2;k=3");
        ce("a;i=1;j=2;k=3,b;i=4;j=5;k=6");

        cq("ab;x=\" \"", "ab;x= ");
        cq("ab;x=\"\\\"\"", "ab;x=\"");

        expect(ext_list{"a,b;i=1,c;j=2;k=3"}.exists("A"));
        expect(ext_list{"a,b;i=1,c;j=2;k=3"}.exists("b"));
        expect(! ext_list{"a,b;i=1,c;j=2;k=3"}.exists("d"));

        // invalid strings
        cs("i j", "i");
        cs(";", "");
    }

    static
    std::string
    str(token_list const& c)
    {
        bool first = true;
        std::string s;
        for(auto const& p : c)
        {
            if(! first)
                s.push_back(',');
            s.append(str(p));
            first = false;
        }
        return s;
    }

    void
    testTokenList()
    {
        auto const ce =
            [&](std::string const& s)
            {
                auto const got = str(token_list{s});
                expect(got == s, fmt(got));
            };
        auto const cs =
            [&](std::string const& s, std::string const& good)
            {
                ce(good);
                auto const got = str(token_list{s});
                ce(got);
                expect(got == good, fmt(got));
            };

        cs("", "");
        cs(" ", "");
        cs("  ", "");
        cs("\t", "");
        cs(" \t ", "");
        cs(",", "");
        cs(",,", "");
        cs(" ,", "");
        cs(" , ,", "");
        cs(" x", "x");
        cs(" \t x", "x");
        cs("x ", "x");
        cs("x \t", "x");
        cs(" \t x \t ", "x");
        ce("x,y");
        cs("x ,\ty ", "x,y");
        cs("x, y, z", "x,y,z");

        expect(token_list{"a,b,c"}.exists("A"));
        expect(token_list{"a,b,c"}.exists("b"));
        expect(! token_list{"a,b,c"}.exists("d"));

        // invalid
        cs("x y", "x");
    }

    void
    run()
    {
        testParamList();
        testExtList();
        testTokenList();
    }
};

BEAST_DEFINE_TESTSUITE(rfc7230,http,beast);

} // test
} // http
} // beast
