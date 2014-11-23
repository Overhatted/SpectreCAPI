//
//  MPTypes.h
//  MasterPassword
//
//  Created by Maarten Billemont on 02/01/12.
//  Copyright (c) 2012 Lyndir. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>

#include <alg/sha256.h>

#include <curses.h>
#include <term.h>

#include "types.h"

const MPElementType TypeWithName(const char *typeName) {
    char lowerTypeName[strlen(typeName)];
    strcpy(lowerTypeName, typeName);
    for (char *tN = lowerTypeName; *tN; ++tN)
        *tN = tolower(*tN);

    if (0 == strcmp(lowerTypeName, "x") || 0 == strcmp(lowerTypeName, "max") || 0 == strcmp(lowerTypeName, "maximum"))
        return MPElementTypeGeneratedMaximum;
    if (0 == strcmp(lowerTypeName, "l") || 0 == strcmp(lowerTypeName, "long"))
        return MPElementTypeGeneratedLong;
    if (0 == strcmp(lowerTypeName, "m") || 0 == strcmp(lowerTypeName, "med") || 0 == strcmp(lowerTypeName, "medium"))
        return MPElementTypeGeneratedMedium;
    if (0 == strcmp(lowerTypeName, "b") || 0 == strcmp(lowerTypeName, "basic"))
        return MPElementTypeGeneratedBasic;
    if (0 == strcmp(lowerTypeName, "s") || 0 == strcmp(lowerTypeName, "short"))
        return MPElementTypeGeneratedShort;
    if (0 == strcmp(lowerTypeName, "i") || 0 == strcmp(lowerTypeName, "pin"))
        return MPElementTypeGeneratedPIN;
    if (0 == strcmp(lowerTypeName, "n") || 0 == strcmp(lowerTypeName, "name"))
        return MPElementTypeGeneratedName;
    if (0 == strcmp(lowerTypeName, "p") || 0 == strcmp(lowerTypeName, "phrase"))
        return MPElementTypeGeneratedPhrase;

    fprintf(stderr, "Not a generated type name: %s", lowerTypeName);
    abort();
}

const char *CipherForType(MPElementType type, uint8_t seedByte) {
    if (!(type & MPElementTypeClassGenerated)) {
        fprintf(stderr, "Not a generated type: %d", type);
        abort();
    }

    switch (type) {
        case MPElementTypeGeneratedMaximum: {
            const char *ciphers[] = { "anoxxxxxxxxxxxxxxxxx", "axxxxxxxxxxxxxxxxxno" };
            return ciphers[seedByte % 2];
        }
        case MPElementTypeGeneratedLong: {
            const char *ciphers[] = { "CvcvnoCvcvCvcv", "CvcvCvcvnoCvcv", "CvcvCvcvCvcvno", "CvccnoCvcvCvcv", "CvccCvcvnoCvcv", "CvccCvcvCvcvno", "CvcvnoCvccCvcv", "CvcvCvccnoCvcv", "CvcvCvccCvcvno", "CvcvnoCvcvCvcc", "CvcvCvcvnoCvcc", "CvcvCvcvCvccno", "CvccnoCvccCvcv", "CvccCvccnoCvcv", "CvccCvccCvcvno", "CvcvnoCvccCvcc", "CvcvCvccnoCvcc", "CvcvCvccCvccno", "CvccnoCvcvCvcc", "CvccCvcvnoCvcc", "CvccCvcvCvccno" };
            return ciphers[seedByte % 21];
        }
        case MPElementTypeGeneratedMedium: {
            const char *ciphers[] = { "CvcnoCvc", "CvcCvcno" };
            return ciphers[seedByte % 2];
        }
        case MPElementTypeGeneratedBasic: {
            const char *ciphers[] = { "aaanaaan", "aannaaan", "aaannaaa" };
            return ciphers[seedByte % 3];
        }
        case MPElementTypeGeneratedShort: {
            return "Cvcn";
        }
        case MPElementTypeGeneratedPIN: {
            return "nnnn";
        }
        case MPElementTypeGeneratedName: {
            return "cvccvcvcv";
        }
        case MPElementTypeGeneratedPhrase: {
            const char *ciphers[] = { "cvcc cvc cvccvcv cvc", "cvc cvccvcvcv cvcv", "cv cvccv cvc cvcvccv" };
            return ciphers[seedByte % 3];
        }
        default: {
            fprintf(stderr, "Unknown generated type: %d", type);
            abort();
        }
    }
}

const MPElementVariant VariantWithName(const char *variantName) {
    char lowerVariantName[strlen(variantName)];
    strcpy(lowerVariantName, variantName);
    for (char *vN = lowerVariantName; *vN; ++vN)
        *vN = tolower(*vN);

    if (0 == strcmp(lowerVariantName, "p") || 0 == strcmp(lowerVariantName, "password"))
        return MPElementVariantPassword;
    if (0 == strcmp(lowerVariantName, "l") || 0 == strcmp(lowerVariantName, "login"))
        return MPElementVariantLogin;
    if (0 == strcmp(lowerVariantName, "a") || 0 == strcmp(lowerVariantName, "answer"))
        return MPElementVariantAnswer;

    fprintf(stderr, "Not a variant name: %s", lowerVariantName);
    abort();
}

const char *ScopeForVariant(MPElementVariant variant) {
    switch (variant) {
        case MPElementVariantPassword: {
            return "com.lyndir.masterpassword";
        }
        case MPElementVariantLogin: {
            return "com.lyndir.masterpassword.login";
        }
        case MPElementVariantAnswer: {
            return "com.lyndir.masterpassword.answer";
        }
        default: {
            fprintf(stderr, "Unknown variant: %d", variant);
            abort();
        }
    }
}

const char CharacterFromClass(char characterClass, uint8_t seedByte) {
    const char *classCharacters;
    switch (characterClass) {
        case 'V': {
            classCharacters = "AEIOU";
            break;
        }
        case 'C': {
            classCharacters = "BCDFGHJKLMNPQRSTVWXYZ";
            break;
        }
        case 'v': {
            classCharacters = "aeiou";
            break;
        }
        case 'c': {
            classCharacters = "bcdfghjklmnpqrstvwxyz";
            break;
        }
        case 'A': {
            classCharacters = "AEIOUBCDFGHJKLMNPQRSTVWXYZ";
            break;
        }
        case 'a': {
            classCharacters = "AEIOUaeiouBCDFGHJKLMNPQRSTVWXYZbcdfghjklmnpqrstvwxyz";
            break;
        }
        case 'n': {
            classCharacters = "0123456789";
            break;
        }
        case 'o': {
            classCharacters = "@&%?,=[]_:-+*$#!'^~;()/.";
            break;
        }
        case 'x': {
            classCharacters = "AEIOUaeiouBCDFGHJKLMNPQRSTVWXYZbcdfghjklmnpqrstvwxyz0123456789!@#$%^&*()";
            break;
        }
        case ' ': {
            classCharacters = " ";
            break;
        }
        default: {
            fprintf(stderr, "Unknown character class: %c", characterClass);
            abort();
         }
    }

    return classCharacters[seedByte % strlen(classCharacters)];
}

const char *IDForBuf(const void *buf, size_t length) {
    uint8_t hash[32];
    SHA256_Buf(buf, length, hash);

    char *id = (char *)calloc(65, sizeof(char));
    for (int kH = 0; kH < 32; kH++)
        sprintf(&(id[kH * 2]), "%02X", hash[kH]);

    return id;
}

const char *Hex(const void *buf, size_t length) {
    char *id = (char *)calloc(length*2+1, sizeof(char));
    for (int kH = 0; kH < length; kH++)
        sprintf(&(id[kH * 2]), "%02X", ((const uint8_t*)buf)[kH]);

    return id;
}

int putvari;
char *putvarc = NULL;
bool istermsetup = false;
static void initputvar() {
    if (putvarc)
        free(putvarc);
    putvarc=(char *)calloc(256, sizeof(char));
    putvari=0;

    if (!istermsetup)
        istermsetup = (OK == setupterm(NULL, STDERR_FILENO, NULL));
}
static int putvar(int c) {
    putvarc[putvari++]=c;
    return 0;
}

const char *Identicon(const char *userName, const char *masterPassword) {
    const char *leftArm[]   = { "╔", "╚", "╰", "═" };
    const char *rightArm[]  = { "╗", "╝", "╯", "═" };
    const char *body[]      = { "█", "░", "▒", "▓", "☺", "☻" };
    const char *accessory[] = { "◈", "◎", "◐", "◑", "◒", "◓", "☀", "☁", "☂", "☃", "☄", "★", "☆", "☎", "☏", "⎈", "⌂", "☘", "☢", "☣", "☕", "⌚", "⌛", "⏰", "⚡", "⛄", "⛅", "☔", "♔", "♕", "♖", "♗", "♘", "♙", "♚", "♛", "♜", "♝", "♞", "♟", "♨", "♩", "♪", "♫", "⚐", "⚑", "⚔", "⚖", "⚙", "⚠", "⌘", "⏎", "✄", "✆", "✈", "✉", "✌" };

    uint8_t identiconSeed[32];
    HMAC_SHA256_Buf(masterPassword, strlen(masterPassword), userName, strlen(userName), identiconSeed);

    bool useColor = 0;
#ifdef COLOR
    if (isatty(STDERR_FILENO))
        useColor = 1;
#endif

    uint8_t colorIdentifier = identiconSeed[4] % 7 + 1;
    char *colorString, *resetString;
    if (useColor) {
        initputvar();
        tputs(tparm(tgetstr("AF", NULL), colorIdentifier), 1, putvar);
        colorString = calloc(strlen(putvarc) + 1, sizeof(char));
        strcpy(colorString, putvarc);
        tputs(tgetstr("me", NULL), 1, putvar);
        resetString = calloc(strlen(putvarc) + 1, sizeof(char));
        strcpy(resetString, putvarc);
    } else {
        colorString = calloc(1, sizeof(char));
        resetString = calloc(1, sizeof(char));
    }

    char *identicon = (char *)calloc(256, sizeof(char));
    snprintf(identicon, 256, "%s%s%s%s%s%s",
             colorString,
             leftArm[identiconSeed[0] % (sizeof(leftArm) / sizeof(leftArm[0]))],
             body[identiconSeed[1] % (sizeof(body) / sizeof(body[0]))],
             rightArm[identiconSeed[2] % (sizeof(rightArm) / sizeof(rightArm[0]))],
             accessory[identiconSeed[3] % (sizeof(accessory) / sizeof(accessory[0]))],
             resetString);

    free(colorString);
    free(resetString);
    return identicon;
}
