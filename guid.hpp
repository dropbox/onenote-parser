#ifndef _GUID_HPP_
#define _GUID_HPP_
enum class error {
  OK,
    SEGV,
    RESERVED_NONZERO,
    UNKNOWN_ENUM,
    INVALID_CONSTANT,
    STRING_TOO_SHORT,
    HEX_OUT_OF_RANGE,
    COMPACT_ID_MISSING,
    UNKNOWN_GUID,
};
static error wrap(error err) {
  fprintf(stderr, "Error %d encountered\n", (int)err);
  printf("Error %d encountered\n", (int)err);
  return err;
}

const error Ok = error::OK;
struct GUID {
  static GUID nil() {
    GUID retval;
    memset(retval.guid, 0, sizeof(retval.guid));
    return retval;
  }
  uint8_t guid[16];
  static void swap16(uint8_t *byte2) {
    uint8_t tmp = byte2[0];
    byte2[0] = byte2[1];
    byte2[1] = tmp;
  }
  static void swap32(uint8_t *byte4) {
    uint8_t tmp = byte4[0];
    byte4[0] = byte4[3];
    byte4[3] = tmp;
    tmp = byte4[1];
    byte4[1] = byte4[2];
    byte4[2] = tmp;
  }
  static error hex_to_nibble(uint16_t nibble, uint8_t *outnibble) {
    if (nibble >= 'a' && nibble <= 'f') {
      *outnibble = (nibble - 'a' + 10);
      return Ok;
    }
    if (nibble >= 'A' && nibble <= 'F') {
      *outnibble = (nibble - 'A' + 10);
      return Ok;
    }
    if (nibble >= '0' && nibble <= '9') {
      *outnibble = nibble - '0';
      return Ok;
    }
    return error::HEX_OUT_OF_RANGE;
  }
  static error hex_to_byte(uint16_t msn, uint16_t lsn, uint8_t *outbyte) {
    uint8_t outmsn = 0;
    uint8_t outlsn = 0;
    error errorA = hex_to_nibble(msn, &outmsn);
    error errorB = hex_to_nibble(lsn, &outlsn);
    if (errorA != Ok) {
      return errorA;
    }
    if (errorB != Ok) {
      return errorB;
    }
    *outbyte = outmsn * 0x10 + outlsn;
    return Ok;
  }
  static error parse(const uint16_t *data, uint64_t length, GUID*output) {
    size_t out_count = 0;
    for (size_t i = 0; i + 1 < length && out_count < 16 ; ++i) {
      uint8_t out_byte = 0;
      error err = hex_to_byte(data[i], data[i + 1], &out_byte);
      if (err != Ok) { // this isn't a hex nibble
	continue;
      }
      output->guid[out_count] = out_byte;
      ++i; // increment by 2
      ++out_count;
    }
    if (out_count != 16) {
      return error::STRING_TOO_SHORT;
    }
    // reshuffle output to match idnE-na-en-ssInGUID
    swap32(&output->guid[0]);
    swap16(&output->guid[4]);
    swap16(&output->guid[6]);
    return Ok;
  }
  static char digit_to_hex(uint8_t v) {
    if (v >= 10) {
      return 'a' + (v - 10);
    }
    return '0' + v;
  }
  bool operator != (const GUID &other) const {
    return memcmp(guid, other.guid, sizeof(guid)) != 0;
  }
  bool operator == (const GUID &other) const {
    return memcmp(guid, other.guid, sizeof(guid)) == 0;
  }
  bool operator < (const GUID &other) const {
    return memcmp(guid, other.guid, sizeof(guid)) < 0;
  }
  std::string toString() const {
    char retval[sizeof(guid) * 2 + 1];
    const uint8_t iter_order[16] = {3,2,1,0,5,4,7,6,8,9,0xa,0xb,0xc,0xd,0xe,0xf};
    for (size_t i = 0;i < sizeof(guid); ++i) {
      uint8_t val = guid[iter_order[i]];
      retval[i * 2] = digit_to_hex(val >> 4);
      retval[i * 2 + 1] = digit_to_hex(val & 0xf);
    }
    retval[sizeof(retval) -1] = '\0';
    return retval;
  }
};

struct ExtendedGUID {
  GUID guid;
  uint32_t n;
  std::string toString()const {
    char data[32];
    sprintf(data, "%08x", n);
    return guid.toString() + ":" + data;
  }
  static ExtendedGUID nil() {
    ExtendedGUID retval;
    retval.guid = GUID::nil();
    retval.n = 0;
    return retval;
  }
  bool operator != (const ExtendedGUID &other) const {
    return n != other.n || guid != other.guid;
  }
  bool operator == (const ExtendedGUID &other) const {
    return n == other.n && guid == other.guid;
  }
  bool operator < (const ExtendedGUID &other) const {
    if (guid == other.guid) {
      return n < other.n;
    }
    return guid < other.guid;
  }
};

#endif
