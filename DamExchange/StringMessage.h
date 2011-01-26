#pragma once
#include <string>

namespace DamExchangeProtocol {

#include <cstdio>
#include <cstdlib>
#include <cstring>

class StringMessage
{
public:
        // constructors
        explicit StringMessage(const std::string&);
        StringMessage(const std::string&, const std::string&);

        // views
        std::string str(void) const;
        std::string header(void) const;
        std::string body(void) const;

        static char terminator(void);

private:
        // invariants
        bool invariant(void) const;
        bool invariant(const std::string&, const std::string&) const;

        // implementation
        static const size_t HEADER_LENGTH = 1;
        static const size_t MAX_BODY_LENGTH = 126;
        static const char TERMINATOR = '\0';

        // representation
        std::string content_;
};

} // namespace DamExchangeProtocol
