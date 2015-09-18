#pragma once
#include <dctl/ui/dxp/message.hpp>      // Message
#include <dctl/utility/factory/creatable.hpp>   // make_creatable
#include <string>                       // string

namespace dctl {
namespace dxp {

/*

        The format and semantics of Chat are defined at:
        http://www.mesander.nl/damexchange/echat.htm

*/

class Chat final
:
        // Curiously Recurring Template Pattern (CRTP)
        public factory::make_creatable<Message, Chat, 'C'>
{
public:
        // constructors

        explicit Chat(std::string const& message)
        :
                text_ { message }
        {}

        // observers

        std::string text() const
        {
                return text_;
        }

        // output

        static std::string str(std::string const& message)
        {
                return identifier() + body(message);
        }

private:
        // virtual implementation

        std::string do_header() const override
        {
                return identifier();
        }

        std::string do_body() const override
        {
                return body(text());
        }

        static std::string body(std::string const& m)
        {
                return m;
        }

        // representation

        std::string text_;
};

}       // namespace dxp
}       // namespace dctl