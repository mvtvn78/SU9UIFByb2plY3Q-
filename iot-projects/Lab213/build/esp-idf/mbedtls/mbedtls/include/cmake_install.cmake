# Install script for directory: F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Lab213")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "F:/Study/IOT/.espressif/tools/riscv32-esp-elf/esp-12.2.0_20230208/riscv32-esp-elf/bin/riscv32-esp-elf-objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mbedtls" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/aes.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/aria.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/asn1.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/asn1write.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/base64.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/bignum.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/block_cipher.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/build_info.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/camellia.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ccm.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/chacha20.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/chachapoly.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/check_config.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/cipher.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/cmac.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/compat-2.x.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_adjust_legacy_crypto.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_adjust_legacy_from_psa.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_adjust_psa_from_legacy.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_adjust_psa_superset_legacy.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_adjust_ssl.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_adjust_x509.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_psa.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/constant_time.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ctr_drbg.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/debug.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/des.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/dhm.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ecdh.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ecdsa.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ecjpake.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ecp.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/entropy.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/error.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/gcm.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/hkdf.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/hmac_drbg.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/lms.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/mbedtls_config.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/md.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/md5.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/memory_buffer_alloc.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/net_sockets.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/nist_kw.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/oid.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pem.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pk.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pkcs12.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pkcs5.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pkcs7.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/platform.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/platform_time.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/platform_util.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/poly1305.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/private_access.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/psa_util.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ripemd160.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/rsa.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/sha1.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/sha256.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/sha3.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/sha512.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl_cache.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl_ciphersuites.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl_cookie.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl_ticket.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/threading.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/timing.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/version.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/x509.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/x509_crl.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/x509_crt.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/mbedtls/x509_csr.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/psa" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/build_info.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/crypto.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_adjust_auto_enabled.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_adjust_config_dependencies.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_adjust_config_key_pair_types.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_adjust_config_synonyms.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_builtin_composites.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_builtin_key_derivation.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_builtin_primitives.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_compat.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_config.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_driver_common.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_driver_contexts_composites.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_driver_contexts_key_derivation.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_driver_contexts_primitives.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_extra.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_legacy.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_platform.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_se_driver.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_sizes.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_struct.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_types.h"
    "F:/Study/IOT/ESP_IDF/v5.1.6/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_values.h"
    )
endif()

