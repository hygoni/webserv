#include "Request.hpp"
#include "abnf.hpp"
#include "cbnf.hpp"
#include "libft.h"
#include "construct.hpp"
// method SP request-target SP HTTP-version CRLF
// field-name ":" OWS field-value OWS

void  Request::initMethod(Context& ctx) {
  size_t start = ctx.current();
  CBNF::Token(ctx);
  this->_method = ctx.str.substr(start, ctx.current());
}

void  Request::initTarget(Context& ctx) {
  //  request-target = origin-form / absolute-form / authority-form / asterisk-form

  //  origin-form = absolute-path [ "?" query ]
  //    absolute-path = 1*( "/" segment )
  //      segment = *pchar
  //    query = *( pchar / "/" / "?" )

  //  absolute-form = absolute-URI
  //    absolute-URI = scheme ":" hier-part [ "?" query ]
  //      scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
  //      hier-part   = "//" authority path-abempty / path-absolute / path-rootless / path-empty
  //        authority     = [ userinfo "@" ] host [ ":" port ]
  //          userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
  //          host          = IP-literal / IPv4address / reg-name
  //            IP-literal = "[" ( IPv6address / IPvFuture  ) "]"
  //              IPv6address = !!!!!!!!!!!!!!!!!!!!!
  //              IPvFuture  = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
  //            IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
  //              dec-octet = !!!!!!!!!!!!!!!!!!!!!
  //            reg-name = *( unreserved / pct-encoded / sub-delims )
  //          port = *DIGIT
  //        path-abempty = *( "/" segment )
  //        path-absolute = "/" [ segment-nz *( "/" segment ) ]
  //          segment-nz = 1*pchar
  //        path-rootless = segment-nz *( "/" segment )
  //        path-empty    = 0<pchar>

  //  authority-form = authority

  //  asterisk-form = "*"
}

Request::Request(std::string http_message) {
  Context ctx(http_message);

  begin(ctx);
  CBNF::Token(ctx);
  accept(ctx, ABNF::Sp);


  accept(ctx, ABNF::Sp);
  accept(ctx, 'H', ABNF::Scar);
  accept(ctx, 'T', ABNF::Scar);
  accept(ctx, 'T', ABNF::Scar);
  accept(ctx, 'P', ABNF::Scar);
  accept(ctx, '/', ABNF::Scar);
  accept(ctx, ABNF::Digit);
  accept(ctx, '.', ABNF::Icar);
  accept(ctx, ABNF::Digit);
  accept(ctx, false, ABNF::Crlf);
  if (ctx.state == false)
    throw std::logic_error("400");

  end(ctx);
}

