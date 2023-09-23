#include "erm/utils/StaticString.h"

namespace erm {

static_assert(str16().empty());
static_assert(str16().size() == 0);
static_assert(str16().capacity() == 16);
static_assert(str16("CIAO").size() == 4);

static_assert(str16("CIAO")[0] == 'C');
static_assert(str16("CIAO")[4] == '\0');

static_assert(str16("CIAO") == str128("CIAO"));
static_assert(str16("CIAO") != str128("CIA"));

static_assert(str16() == "");
static_assert(str16("CIAO") == "CIAO");
static_assert(str16("CIAO") == str32("CIAO"));
static_assert(StaticString<2>("CIAO") == "C");

static_assert(str16("CIAO").startsWith("CI"));
static_assert(str16("CIAO").endsWith("AO"));

static_assert(str16("CIAO").substr(0, 1) == "C");

static_assert(*str16("CIAO").begin() == 'C');
static_assert(*str16("CIAO").end() == '\0');

static_assert(str16("CIAO").find('C') == 0);
static_assert(str16("CIAO").find('O') == 3);
static_assert(str16("CIAO").find('Q') == str16::npos);
static_assert(str16("CIAO").find("IA") == 1);
static_assert(str16("CIAOCIO").find("CIO") == 4);
static_assert(str16("CIAOCIO").find("CIAOD") == str16::npos);
static_assert(str16("CIAO").find("CIAOOO") == str16::npos);

static_assert(str16("CIAO").rfind('C') == 0);
static_assert(str16("CIAO").rfind('O') == 3);
static_assert(str16("CIAO").rfind('Q') == str16::npos);
static_assert(str16("CIAO").rfind("IA") == 1);
static_assert(str16("CIAOCIO").rfind("CIO") == 4);
static_assert(str16("CIAOCIO").rfind("CIAOD") == str16::npos);
static_assert(str16("CIAO").rfind("CIAOOO") == str16::npos);
static_assert(str16("CIAOO").rfind('O') == 4);
static_assert(str16("CIAOOAAA").rfind('O') == 4);
static_assert(str16("CIAOOAAA").rfind("OA") == 4);

static_assert(str16("    ").trim() == "");
static_assert(str16("A    ").trim() == "A");
static_assert(str16("    A").trim() == "A");
static_assert(str16("  CIAO ").trim() == "CIAO");
static_assert(str16("  CIAO").trim() == "CIAO");
static_assert(str16("CIAO ").trim() == "CIAO");
static_assert(str16("                  ").trim() == "");

}