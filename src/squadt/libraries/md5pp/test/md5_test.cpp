#include "md5pp.h"
#include <cstdlib>
#include <sstream>

/*
 * those are the standard RFC 1321 test vectors
 */

static char *msg[] = 
{
    "",
    "a",
    "abc",
    "message digest",
    "abcdefghijklmnopqrstuvwxyz",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
    "12345678901234567890123456789012345678901234567890123456789012" \
        "345678901234567890"
};

static char *val[] =
{
    "d41d8cd98f00b204e9800998ecf8427e",
    "0cc175b9c0f1b6a831c399e269772661",
    "900150983cd24fb0d6963f7d28e17f72",
    "f96b697d7cb7938d525a2f31aaf161d0",
    "c3fcd3d76192e4007dfb496cca67e13b",
    "d174ab98d277d9f5a5611c2c9f419d9f",
    "57edf4a22be3c955ac49da2e2107b67a"
};

int main( int argc, char *argv[] )
{
    std::cerr << "\n MD5 Validation Tests:\n\n";

    for( unsigned int i = 0; i < 7; i++ )
    {
        md5::MD5 sum;
        std::stringstream string;

        std::cerr << " Test " << (i + 1);

        sum.Update((uint8_t *) msg[i], strlen( msg[i] ) );
        sum.Finalise();

        string << sum;

        if( memcmp( string.str().c_str(), val[i], 32 ) )
        {
          std::cerr << " failed.\n";;
          return( 1 );
        }

        std::cerr << " passed.\n";;
    }

    std::cerr << std::endl;

    return( 0 );
}

