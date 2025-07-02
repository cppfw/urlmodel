/*
MIT License

Copyright (c) 2023 Ivan Gagis <igagis@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/* ================ LICENSE END ================ */

#include "parser.hpp"

#include <utki/string.hpp>

using namespace urlmodel;

utki::span<const uint8_t> parser::parse_scheme(utki::span<const uint8_t> data) {
  auto i = data.begin();
  for (; i != data.end(); ++i) {
    auto c = char(*i);

    c = std::tolower(c, std::locale::classic());

    if (c == '/') {
      // start parsing path without scheme and authority
      this->cur_state = state::path;
      ++i;
      break;
    } else if (c == ':') {
      // end of scheme
      this->url.scheme = utki::make_string(this->buf);
      this->buf.clear();
      this->cur_state = state::authority_prefix;
      ++i;
      break;
    }

    if (this->buf.empty()) {
      if (!std::isalpha(c, std::locale::classic())) {
        std::stringstream ss;
        ss << "urlmodel: first char of URL scheme is not alphabethic: " << c;
        throw std::invalid_argument(ss.str());
      }
    } else {
      if (!(std::isalpha(c, std::locale::classic()) ||
            std::isdigit(c, std::locale::classic()) || c == '+' || c == '-' ||
            c == '.')) {
        std::stringstream ss;
        ss << "urlmodel: URL scheme contains forbidden character: " << c;
        throw std::invalid_argument(ss.str());
      }
    }

    this->buf.push_back(c);
  }
  data = data.subspan(std::distance(data.begin(), i));
  return data;
}

utki::span<const uint8_t>
parser::parse_authority_prefix(utki::span<const uint8_t> data) {
  auto i = data.begin();
  for (; i != data.end(); ++i) {
    auto c = char(*i);

    if (c != '/') {
      if (std::isspace(c, std::locale::classic())) {
        ASSERT(this->buf.empty() ||
               (this->buf.size() == 1 && this->buf[0] == '/'))
        this->cur_state = state::end;
        break;
      } else if (c == '?') {
        ASSERT(this->buf.empty() ||
               (this->buf.size() == 1 && this->buf[0] == '/'))

        // the path will be empty in any case, even if one '/' was present
        // before '?'

        this->buf.clear();
        this->cur_state = state::query_name;
        ++i;
        break;
      } else if (c == '#') {
        ASSERT(this->buf.empty() ||
               (this->buf.size() == 1 && this->buf[0] == '/'))

        // the path will be empty in any case, even if one '/' was present
        // before '?'

        this->buf.clear();
        this->cur_state = state::fragment;
        ++i;
        break;
      } else if (this->buf.size() == 1) {
        ASSERT(this->buf[0] == '/')
        // path starts with single '/' character
        this->buf.clear();
        this->cur_state = state::path;
        break;
      }
      throw std::invalid_argument(
          "urlmodel: authority must start with // or be absent");
    }

    ASSERT(this->buf.size() <= 1)

    if (this->buf.empty()) {
      this->buf.push_back('/');
      continue;
    }

    ASSERT(this->buf.size() == 1)
    ASSERT(this->buf[0] == '/')

    this->buf.clear();
    this->cur_state = state::authority;
    ++i;
    break;
  }
  data = data.subspan(std::distance(data.begin(), i));
  return data;
}

void parser::handle_end_of_authority() {
  auto str = utki::make_string_view(this->buf);

  auto at_pos = str.find('@');
  if (at_pos != std::string_view::npos) {
    ASSERT(at_pos < str.size())
    // there is user info in the authority

    utki::string_parser p(str.substr(0, at_pos));

    this->url.username = p.read_chars_until(':');

    if (!p.empty()) {
      // skip ':'
      p.read_char();

      this->url.password = p.read_chars(p.size());
    }

    str = str.substr(at_pos + 1);
  }

  utki::string_parser p(str);

  this->url.host = p.read_chars_until(':');

  if (!p.empty()) {
    // skip ':'
    p.read_char();

    this->url.port = p.read_number<uint16_t>();

    if (this->url.port == 0) {
      throw std::invalid_argument("urlmodel: invalid port specified in URL: 0");
    }
  }

  this->buf.clear();
}

utki::span<const uint8_t>
parser::parse_authority(utki::span<const uint8_t> data) {
  auto i = data.begin();
  for (; i != data.end(); ++i) {
    auto c = char(*i);

    if (std::isspace(c, std::locale::classic())) {
      this->handle_end_of_authority();
      this->cur_state = state::end;
      break;
    } else if (c == '/') {
      this->handle_end_of_authority();
      this->cur_state = state::path;
      ++i;
      break;
    } else if (c == '?') {
      this->handle_end_of_authority();
      this->cur_state = state::query_name;
      ++i;
      break;
    } else if (c == '#') {
      this->handle_end_of_authority();
      this->cur_state = state::fragment;
      ++i;
      break;
    }

    this->buf.push_back(c);
  }
  data = data.subspan(std::distance(data.begin(), i));
  return data;
}

void parser::handle_end_of_path_segment() {
  if (this->buf.empty()) {
    return;
  }

  this->url.path.push_back(utki::make_string(this->buf));
  this->buf.clear();
}

utki::span<const uint8_t> parser::parse_path(utki::span<const uint8_t> data) {
  auto i = data.begin();
  for (; i != data.end(); ++i) {
    auto c = char(*i);

    if (std::isspace(c, std::locale::classic())) {
      this->handle_end_of_path_segment();
      this->cur_state = state::end;
      break;
    } else if (c == '/') {
      this->handle_end_of_path_segment();
      continue;
    } else if (c == '?') {
      this->handle_end_of_path_segment();
      this->cur_state = state::query_name;
      ++i;
      break;
    } else if (c == '#') {
      this->handle_end_of_path_segment();
      this->cur_state = state::fragment;
      ++i;
      break;
    }

    this->buf.push_back(c);
  }
  data = data.subspan(std::distance(data.begin(), i));
  return data;
}

utki::span<const uint8_t>
parser::parse_query_name(utki::span<const uint8_t> data) {
  auto i = data.begin();
  for (; i != data.end(); ++i) {
    auto c = char(*i);

    if (std::isspace(c, std::locale::classic())) {
      throw std::invalid_argument(
          "urlmodel: unexpected end of URL while parsing query parameter name");
    } else if (c == '=') {
      this->parsed_query_name = utki::make_string(this->buf);
      this->buf.clear();
      this->cur_state = state::query_value;
      ++i;
      break;
    }

    this->buf.push_back(c);
  }
  data = data.subspan(std::distance(data.begin(), i));
  return data;
}

void parser::handle_end_of_query_value() {
  this->url.query.insert_or_assign(std::move(this->parsed_query_name),
                                   utki::make_string(this->buf));
  this->parsed_query_name.clear();
  this->buf.clear();
}

utki::span<const uint8_t>
parser::parse_query_value(utki::span<const uint8_t> data) {
  auto i = data.begin();
  for (; i != data.end(); ++i) {
    auto c = char(*i);

    if (std::isspace(c, std::locale::classic())) {
      this->handle_end_of_query_value();
      this->cur_state = state::end;
      break;
    } else if (c == '&') {
      this->handle_end_of_query_value();
      this->cur_state = state::query_name;
      ++i;
      break;
    } else if (c == '#') {
      this->handle_end_of_query_value();
      this->cur_state = state::fragment;
      ++i;
      break;
    }

    this->buf.push_back(c);
  }
  data = data.subspan(std::distance(data.begin(), i));
  return data;
}

utki::span<const uint8_t>
parser::parse_fragment(utki::span<const uint8_t> data) {
  auto i = data.begin();
  for (; i != data.end(); ++i) {
    auto c = char(*i);

    if (std::isspace(c, std::locale::classic())) {
      this->url.fragment = utki::make_string(this->buf);
      this->cur_state = state::end;
      break;
    }

    this->buf.push_back(c);
  }
  data = data.subspan(std::distance(data.begin(), i));
  return data;
}

void parser::end_of_data() {
  // feed in whitespace which will indicate URL end
  uint8_t space = ' ';
  this->feed(utki::make_span(&space, 1));
}

utki::span<const uint8_t> parser::feed(utki::span<const uint8_t> data) {
  while (!data.empty()) {
    switch (this->cur_state) {
    case state::scheme:
      data = this->parse_scheme(data);
      break;
    case state::authority_prefix:
      data = this->parse_authority_prefix(data);
      break;
    case state::authority:
      data = this->parse_authority(data);
      break;
    case state::path:
      data = this->parse_path(data);
      break;
    case state::query_name:
      data = this->parse_query_name(data);
      break;
    case state::query_value:
      data = this->parse_query_value(data);
      break;
    case state::fragment:
      data = this->parse_fragment(data);
      break;
    case state::end:
      return data;
    }
  }
  return data;
}
