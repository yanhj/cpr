
#include "cpr/ssl_ctx.h"

#if SUPPORT_CURLOPT_SSL_CTX_FUNCTION

#ifdef OPENSSL_BACKEND_USED

#include <iostream>
#include <openssl/err.h>
#include <openssl/safestack.h>
#include <openssl/ssl.h>

namespace cpr {

/**
 * The ssl_ctx parameter is actually a pointer to the SSL library's SSL_CTX for OpenSSL.
 * If an error is returned from the callback no attempt to establish a connection is made and
 * the perform operation will return the callback's error code.
 *
 * Sources: https://curl.se/libcurl/c/CURLOPT_SSL_CTX_FUNCTION.html
 *          https://curl.se/libcurl/c/CURLOPT_SSL_CTX_DATA.html
 */
CURLcode sslctx_function_load_ca_cert_from_buffer(CURL* /*curl*/, void* sslctx, void* raw_cert_buf) {
    // Check arguments
    if (!raw_cert_buf || !sslctx) {
        std::cerr << "Invalid callback arguments\n";
        return CURLE_ABORTED_BY_CALLBACK;
    }
    // Setup pointer
    X509_STORE* store = SSL_CTX_get_cert_store(static_cast<SSL_CTX*>(sslctx));
    STACK_OF(X509_INFO)* certs = nullptr;
    BIO* bio = nullptr;
    const std::string* cert_buf = static_cast<std::string*>(raw_cert_buf);

    // Create a memory BIO using the data of cert_buf.
    // Note: It is assumed, that cert_buf is nul terminated and its length is determined by strlen.
    bio = BIO_new_mem_buf(cert_buf->data(), static_cast<int>(cert_buf->length()));

    // Load the PEM formatted certificate into an X509 structure which OpenSSL can use.
    certs = PEM_X509_INFO_read_bio(bio, nullptr, nullptr, nullptr);
    if (!certs) {
        std::cerr << "PEM_X509_INFO_read_bio failed\n";
        BIO_free(bio);
        return CURLE_ABORTED_BY_CALLBACK;
    }

    int status = 0;
    for (int i = 0; i < sk_X509_INFO_num(certs); i++) {
        // NOLINTNEXTLINE (cppcoreguidelines-pro-type-cstyle-cast)
        X509_INFO* itmp = sk_X509_INFO_value(certs, i);
        if (itmp->x509) {
            status = X509_STORE_add_cert(store, itmp->x509);
            if (status == 0) {
                std::cerr << "Error adding certificate\n";
                sk_X509_INFO_pop_free(certs, X509_INFO_free);
                BIO_free(bio);
                return CURLE_ABORTED_BY_CALLBACK;
            }
        }
        if (itmp->crl) {
            status = X509_STORE_add_crl(store, itmp->crl);
            if (status == 0) {
                std::cerr << "Error adding certificate\n";
                sk_X509_INFO_pop_free(certs, X509_INFO_free);
                BIO_free(bio);
                return CURLE_ABORTED_BY_CALLBACK;
            }
        }
    }

    sk_X509_INFO_pop_free(certs, X509_INFO_free);
    BIO_free(bio);

    // The CA certificate was loaded successfully into the verification storage
    return CURLE_OK;
}

} // namespace cpr

#endif // OPENSSL_BACKEND_USED

#endif // SUPPORT_CURLOPT_SSL_CTX_FUNCTION