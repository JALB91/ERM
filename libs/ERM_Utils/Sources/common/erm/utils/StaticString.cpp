#include "erm/utils/StaticString.h"

namespace erm {

static_assert(str16().empty());

static_assert(str16().capacity() == 16);
static_assert(str16("CIAO").size() == 4);

static_assert(str16("CIAO")[0] == 'C');
static_assert(str16("CIAO")[4] == '\0');

static_assert(str16() == "");
static_assert(str16("CIAO") == "CIAO");
static_assert(str16("CIAO") == str32("CIAO"));
static_assert(StaticString<2>("CIAO") == "C");

static_assert(str16("CIAO").substr(0, 1) == "C");

static_assert(*str16("CIAO").begin() == 'C');
static_assert(*str16("CIAO").end() == '\0');

}