//==============================================================================
// This file is part of Master Password.
// Copyright (c) 2011-2017, Maarten Billemont.
//
// Master Password is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Master Password is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You can find a copy of the GNU General Public License in the
// LICENSE file.  Alternatively, see <http://www.gnu.org/licenses/>.
//==============================================================================

#include "mpw-algorithm_v2.h"
#include "mpw-util.h"

MP_LIBS_BEGIN
#include <string.h>
#include <errno.h>
#include <time.h>
MP_LIBS_END

#define MP_N                32768LU
#define MP_r                8U
#define MP_p                2U
#define MP_otp_window       5 * 60 /* s */

// Algorithm version overrides.
bool mpw_master_key_v2(
        const MPMasterKey *masterKey, const char *fullName, const char *masterPassword) {

    return mpw_master_key_v1( masterKey, fullName, masterPassword );
}

bool mpw_service_key_v2(
        const MPServiceKey *serviceKey, const MPMasterKey *masterKey, const char *serviceName,
        MPCounterValue keyCounter, MPKeyPurpose keyPurpose, const char *keyContext) {

    const char *keyScope = mpw_purpose_scope( keyPurpose );
    trc( "keyScope: %s", keyScope );

    // OTP counter value.
    if (keyCounter == MPCounterValueTOTP)
        keyCounter = ((uint32_t)time( NULL ) / MP_otp_window) * MP_otp_window;

    // Calculate the service seed.
    char serviceNameHex[9], keyCounterHex[9], keyContextHex[9];
    trc( "serviceSalt: keyScope=%s | #serviceName=%s | serviceName=%s | keyCounter=%s | #keyContext=%s | keyContext=%s",
            keyScope, mpw_hex_l( (uint32_t)strlen( serviceName ), serviceNameHex ), serviceName, mpw_hex_l( keyCounter, keyCounterHex ),
            keyContext? mpw_hex_l( (uint32_t)strlen( keyContext ), keyContextHex ): NULL, keyContext );
    size_t serviceSaltSize = 0;
    uint8_t *serviceSalt = NULL;
    if (!(mpw_push_string( &serviceSalt, &serviceSaltSize, keyScope ) &&
          mpw_push_int( &serviceSalt, &serviceSaltSize, (uint32_t)strlen( serviceName ) ) &&
          mpw_push_string( &serviceSalt, &serviceSaltSize, serviceName ) &&
          mpw_push_int( &serviceSalt, &serviceSaltSize, keyCounter ) &&
          (!keyContext? true:
           mpw_push_int( &serviceSalt, &serviceSaltSize, (uint32_t)strlen( keyContext ) ) &&
           mpw_push_string( &serviceSalt, &serviceSaltSize, keyContext ))) || !serviceSalt) {
        err( "Could not allocate service salt: %s", strerror( errno ) );
        return false;
    }
    trc( "  => serviceSalt.id: %s", mpw_id_buf( serviceSalt, serviceSaltSize ).hex );

    trc( "serviceKey: hmac-sha256( masterKey.id=%s, serviceSalt )", mpw_id_buf( masterKey->bytes, sizeof( masterKey->bytes ) ).hex );
    bool success = mpw_hash_hmac_sha256( (uint8_t *)serviceKey->bytes, masterKey->bytes, sizeof( masterKey->bytes ), serviceSalt, serviceSaltSize );
    mpw_free( &serviceSalt, serviceSaltSize );

    if (!success)
        err( "Could not derive service key: %s", strerror( errno ) );
    else
        trc( "  => serviceKey.id: %s (algorithm: %d=1)", mpw_id_buf( serviceKey->bytes, sizeof( serviceKey->bytes ) ).hex, masterKey->algorithm );
    return success;
}

const char *mpw_service_template_password_v2(
        const MPMasterKey *masterKey, const MPServiceKey *serviceKey, MPResultType resultType, const char *resultParam) {

    return mpw_service_template_password_v1( masterKey, serviceKey, resultType, resultParam );
}

const char *mpw_service_crypted_password_v2(
        const MPMasterKey *masterKey, const MPServiceKey *serviceKey, MPResultType resultType, const char *cipherText) {

    return mpw_service_crypted_password_v1( masterKey, serviceKey, resultType, cipherText );
}

const char *mpw_service_derived_password_v2(
        const MPMasterKey *masterKey, const MPServiceKey *serviceKey, MPResultType resultType, const char *resultParam) {

    return mpw_service_derived_password_v1( masterKey, serviceKey, resultType, resultParam );
}

const char *mpw_service_state_v2(
        const MPMasterKey *masterKey, const MPServiceKey *serviceKey, MPResultType resultType, const char *state) {

    return mpw_service_state_v1( masterKey, serviceKey, resultType, state );
}
