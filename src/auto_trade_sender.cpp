#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "feature_utils.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

httplib::Headers poster_headers({
    {"Cookie", rblx_cookie},
    {"X-CRSF-TOKEN", ""}
});

