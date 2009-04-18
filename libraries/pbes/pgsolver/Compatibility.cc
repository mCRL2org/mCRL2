#include <assert.h>
#include <ctype.h>

#include "Compatibility.h"

static struct CompatibilityChecks {
    CompatibilityChecks() {
        assert(sizeof(int8_t)   == 1);
        assert(sizeof(uint8_t)  == 1);
        assert(sizeof(int16_t)  == 2);
        assert(sizeof(uint16_t) == 2);
        assert(sizeof(int32_t)  == 4);
        assert(sizeof(uint32_t) == 4);
        assert(sizeof(int64_t)  == 8);
        assert(sizeof(uint64_t) == 8);
    }
} checks;

int compat_strcasecmp(const char *s1, const char *s2)
{
    int d = 0;
    while (*s1 && (d = tolower(*s1++) - tolower(*s2++)) == 0);
    return d;
}

int compat_strncasecmp(const char *s1, const char *s2, size_t n)
{
    int d = 0;
    while (n-- > 0 && *s1 && (d = tolower(*s1++) - tolower(*s2++)) == 0);
    return d;
}
