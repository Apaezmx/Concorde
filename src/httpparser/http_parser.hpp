//
//  http_parser.hpp
//  Concorde
//
//  Created by Andres Paez Martinez on 2/14/19.
//  Copyright © 2019 Andres Paez Martinez. All rights reserved.
//

#ifndef http_parser_hpp
#define http_parser_hpp

#include <sys/types.h>

#ifdef _MSC_VER
#define ssize_t intptr_t
#endif

/* $Id$ */

#ifdef __cplusplus
extern "C" {
#endif
    
    /* contains name and value of a header (name == NULL if is a continuing line
     * of a multiline header */
    struct phr_header {
        const char *name;
        size_t name_len;
        const char *value;
        size_t value_len;
    };
    
    /* returns number of bytes consumed if successful, -2 if request is partial,
     * -1 if failed */
    int phr_parse_request(const char *buf, size_t len, const char **method, size_t *method_len, const char **path, size_t *path_len,
                          int *minor_version, struct phr_header *headers, size_t *num_headers, size_t last_len);
    
    /* ditto */
    int phr_parse_response(const char *_buf, size_t len, int *minor_version, int *status, const char **msg, size_t *msg_len,
                           struct phr_header *headers, size_t *num_headers, size_t last_len);
    
    /* ditto */
    int phr_parse_headers(const char *buf, size_t len, struct phr_header *headers, size_t *num_headers, size_t last_len);
    
    /* should be zero-filled before start */
    struct phr_chunked_decoder {
        size_t bytes_left_in_chunk; /* number of bytes left in current chunk */
        char consume_trailer;       /* if trailing headers should be consumed */
        char _hex_count;
        char _state;
    };
    
    /* the function rewrites the buffer given as (buf, bufsz) removing the chunked-
     * encoding headers.  When the function returns without an error, bufsz is
     * updated to the length of the decoded data available.  Applications should
     * repeatedly call the function while it returns -2 (incomplete) every time
     * supplying newly arrived data.  If the end of the chunked-encoded data is
     * found, the function returns a non-negative number indicating the number of
     * octets left undecoded at the tail of the supplied buffer.  Returns -1 on
     * error.
     */
    ssize_t phr_decode_chunked(struct phr_chunked_decoder *decoder, char *buf, size_t *bufsz);
    
    /* returns if the chunked decoder is in middle of chunked data */
    int phr_decode_chunked_is_in_data(struct phr_chunked_decoder *decoder);
    
#ifdef __cplusplus
}
#endif

#endif /* http_parser_hpp */
