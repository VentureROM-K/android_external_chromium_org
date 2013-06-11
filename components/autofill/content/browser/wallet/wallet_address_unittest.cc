// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/json/json_reader.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"
#include "components/autofill/browser/autofill_common_test.h"
#include "components/autofill/browser/autofill_profile.h"
#include "components/autofill/content/browser/wallet/wallet_address.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {

const char kAddressMissingObjectId[] =
    "{"
    "  \"phone_number\":\"phone_number\","
    "  \"postal_address\":"
    "  {"
    "    \"recipient_name\":\"recipient_name\","
    "    \"address_line\":"
    "    ["
    "      \"address_line_1\","
    "      \"address_line_2\""
    "    ],"
    "    \"locality_name\":\"locality_name\","
    "    \"administrative_area_name\":\"administrative_area_name\","
    "    \"postal_code_number\":\"postal_code_number\","
    "    \"country_name_code\":\"country_name_code\""
    "  }"
    "}";

const char kAddressMissingCountryNameCode[] =
    "{"
    "  \"id\":\"id\","
    "  \"phone_number\":\"phone_number\","
    "  \"postal_address\":"
    "  {"
    "    \"recipient_name\":\"recipient_name\","
    "    \"address_line\":"
    "    ["
    "      \"address_line_1\","
    "      \"address_line_2\""
    "    ],"
    "    \"locality_name\":\"locality_name\","
    "    \"administrative_area_name\":\"administrative_area_name\","
    "    \"postal_code_number\":\"postal_code_number\""
    "  }"
    "}";

const char kAddressMissingRecipientName[] =
    "{"
    "  \"id\":\"id\","
    "  \"phone_number\":\"phone_number\","
    "  \"postal_address\":"
    "  {"
    "    \"address_line\":"
    "    ["
    "      \"address_line_1\","
    "      \"address_line_2\""
    "    ],"
    "    \"locality_name\":\"locality_name\","
    "    \"administrative_area_name\":\"administrative_area_name\","
    "    \"postal_code_number\":\"postal_code_number\","
    "    \"country_name_code\":\"country_name_code\""
    "  }"
    "}";

const char kAddressMissingPostalCodeNumber[] =
    "{"
    "  \"id\":\"id\","
    "  \"phone_number\":\"phone_number\","
    "  \"postal_address\":"
    "  {"
    "    \"recipient_name\":\"recipient_name\","
    "    \"address_line\":"
    "    ["
    "      \"address_line_1\","
    "      \"address_line_2\""
    "    ],"
    "    \"locality_name\":\"locality_name\","
    "    \"administrative_area_name\":\"administrative_area_name\","
    "    \"country_name_code\":\"country_name_code\""
    "  }"
    "}";

const char kValidAddress[] =
    "{"
    "  \"id\":\"id\","
    "  \"phone_number\":\"phone_number\","
    "  \"is_minimal_address\":true,"
    "  \"postal_address\":"
    "  {"
    "    \"recipient_name\":\"recipient_name\","
    "    \"address_line\":"
    "    ["
    "      \"address_line_1\","
    "      \"address_line_2\""
    "    ],"
    "    \"locality_name\":\"locality_name\","
    "    \"administrative_area_name\":\"administrative_area_name\","
    "    \"country_name_code\":\"country_name_code\","
    "    \"postal_code_number\":\"postal_code_number\""
    "  }"
    "}";

const char kClientAddressMissingCountryCode[] =
  "{"
  "  \"name\":\"name\","
  "  \"address1\":\"address1\","
  "  \"address2\":\"address2\","
  "  \"city\":\"city\","
  "  \"state\":\"state\","
  "  \"postal_code\":\"postal_code\","
  "  \"phone_number\":\"phone_number\""
  "}";

const char kClientAddressMissingPostalCode[] =
  "{"
  "  \"name\":\"name\","
  "  \"address1\":\"address1\","
  "  \"address2\":\"address2\","
  "  \"city\":\"city\","
  "  \"state\":\"state\","
  "  \"phone_number\":\"phone_number\","
  "  \"country_code\":\"country_code\""
  "}";

const char kClientAddressMissingName[] =
  "{"
  "  \"address1\":\"address1\","
  "  \"address2\":\"address2\","
  "  \"city\":\"city\","
  "  \"state\":\"state\","
  "  \"postal_code\":\"postal_code\","
  "  \"phone_number\":\"phone_number\","
  "  \"country_code\":\"country_code\""
  "}";

const char kClientValidAddress[] =
  "{"
  "  \"name\":\"name\","
  "  \"address1\":\"address1\","
  "  \"address2\":\"address2\","
  "  \"city\":\"city\","
  "  \"state\":\"state\","
  "  \"postal_code\":\"postal_code\","
  "  \"phone_number\":\"phone_number\","
  "  \"country_code\":\"country_code\","
  "  \"type\":\"FULL\""
  "}";

}  // anonymous namespace

namespace autofill {
namespace wallet {

class WalletAddressTest : public testing::Test {
 public:
  WalletAddressTest() {}
 protected:
  void SetUpDictionary(const std::string& json) {
    scoped_ptr<Value> value(base::JSONReader::Read(json));
    DCHECK(value.get());
    DCHECK(value->IsType(Value::TYPE_DICTIONARY));
    dict_.reset(static_cast<DictionaryValue*>(value.release()));
  }
  scoped_ptr<const DictionaryValue> dict_;
};

TEST_F(WalletAddressTest, AddressEqualsIgnoreID) {
  Address address1("country_name_code",
                   ASCIIToUTF16("recipient_name"),
                   ASCIIToUTF16("address_line_1"),
                   ASCIIToUTF16("address_line_2"),
                   ASCIIToUTF16("locality_name"),
                   ASCIIToUTF16("administrative_area_name"),
                   ASCIIToUTF16("postal_code_number"),
                   ASCIIToUTF16("phone_number"),
                   "id1");
  // Same as address1, only id is different.
  Address address2("country_name_code",
                   ASCIIToUTF16("recipient_name"),
                   ASCIIToUTF16("address_line_1"),
                   ASCIIToUTF16("address_line_2"),
                   ASCIIToUTF16("locality_name"),
                   ASCIIToUTF16("administrative_area_name"),
                   ASCIIToUTF16("postal_code_number"),
                   ASCIIToUTF16("phone_number"),
                   "id2");
  // Has same id as address1, but name is different.
  Address address3("country_name_code",
                   ASCIIToUTF16("a_different_name"),
                   ASCIIToUTF16("address_line_1"),
                   ASCIIToUTF16("address_line_2"),
                   ASCIIToUTF16("locality_name"),
                   ASCIIToUTF16("administrative_area_name"),
                   ASCIIToUTF16("postal_code_number"),
                   ASCIIToUTF16("phone_number"),
                   "id1");
  // Same as address1, but no id.
  Address address4("country_name_code",
                   ASCIIToUTF16("recipient_name"),
                   ASCIIToUTF16("address_line_1"),
                   ASCIIToUTF16("address_line_2"),
                   ASCIIToUTF16("locality_name"),
                   ASCIIToUTF16("administrative_area_name"),
                   ASCIIToUTF16("postal_code_number"),
                   ASCIIToUTF16("phone_number"),
                   std::string());

  // Compare the address has id field to itself.
  EXPECT_EQ(address1, address1);
  EXPECT_TRUE(address1.EqualsIgnoreID(address1));

  // Compare the address has no id field to itself
  EXPECT_EQ(address4, address4);
  EXPECT_TRUE(address4.EqualsIgnoreID(address4));

  // Compare two addresses with different id.
  EXPECT_NE(address1, address2);
  EXPECT_TRUE(address1.EqualsIgnoreID(address2));
  EXPECT_TRUE(address2.EqualsIgnoreID(address1));

  // Compare two different addresses.
  EXPECT_NE(address1, address3);
  EXPECT_FALSE(address1.EqualsIgnoreID(address3));
  EXPECT_FALSE(address3.EqualsIgnoreID(address1));

  // Compare two same addresses, one has id, the other doesn't.
  EXPECT_NE(address1, address4);
  EXPECT_TRUE(address1.EqualsIgnoreID(address4));
  EXPECT_TRUE(address4.EqualsIgnoreID(address1));
}

TEST_F(WalletAddressTest, CreateAddressMissingObjectId) {
  SetUpDictionary(kAddressMissingObjectId);
  Address address("country_name_code",
                  ASCIIToUTF16("recipient_name"),
                  ASCIIToUTF16("address_line_1"),
                  ASCIIToUTF16("address_line_2"),
                  ASCIIToUTF16("locality_name"),
                  ASCIIToUTF16("administrative_area_name"),
                  ASCIIToUTF16("postal_code_number"),
                  ASCIIToUTF16("phone_number"),
                  std::string());
  EXPECT_EQ(address, *Address::CreateAddress(*dict_));
}

TEST_F(WalletAddressTest, CreateAddressWithIDMissingObjectId) {
  SetUpDictionary(kAddressMissingObjectId);
  EXPECT_EQ(NULL, Address::CreateAddressWithID(*dict_).get());
}

TEST_F(WalletAddressTest, CreateAddressMissingCountryNameCode) {
  SetUpDictionary(kAddressMissingCountryNameCode);
  EXPECT_EQ(NULL, Address::CreateAddress(*dict_).get());
  EXPECT_EQ(NULL, Address::CreateAddressWithID(*dict_).get());
}

TEST_F(WalletAddressTest, CreateAddressMissingRecipientName) {
  SetUpDictionary(kAddressMissingRecipientName);
  EXPECT_EQ(NULL, Address::CreateAddress(*dict_).get());
  EXPECT_EQ(NULL, Address::CreateAddressWithID(*dict_).get());
}

TEST_F(WalletAddressTest, CreateAddressMissingPostalCodeNumber) {
  SetUpDictionary(kAddressMissingPostalCodeNumber);
  EXPECT_EQ(NULL, Address::CreateAddress(*dict_).get());
  EXPECT_EQ(NULL, Address::CreateAddressWithID(*dict_).get());
}

TEST_F(WalletAddressTest, CreateAddressWithID) {
  SetUpDictionary(kValidAddress);
  Address address("country_name_code",
                  ASCIIToUTF16("recipient_name"),
                  ASCIIToUTF16("address_line_1"),
                  ASCIIToUTF16("address_line_2"),
                  ASCIIToUTF16("locality_name"),
                  ASCIIToUTF16("administrative_area_name"),
                  ASCIIToUTF16("postal_code_number"),
                  ASCIIToUTF16("phone_number"),
                  "id");
  address.set_is_complete_address(false);
  EXPECT_EQ(address, *Address::CreateAddress(*dict_));
  EXPECT_EQ(address, *Address::CreateAddressWithID(*dict_));
}

TEST_F(WalletAddressTest, CreateDisplayAddressMissingCountryNameCode) {
  SetUpDictionary(kClientAddressMissingCountryCode);
  EXPECT_EQ(NULL, Address::CreateDisplayAddress(*dict_).get());
}

TEST_F(WalletAddressTest, CreateDisplayAddressMissingName) {
  SetUpDictionary(kClientAddressMissingName);
  EXPECT_EQ(NULL, Address::CreateDisplayAddress(*dict_).get());
}

TEST_F(WalletAddressTest, CreateDisplayAddressMissingPostalCode) {
  SetUpDictionary(kClientAddressMissingPostalCode);
  EXPECT_EQ(NULL, Address::CreateDisplayAddress(*dict_).get());
}

TEST_F(WalletAddressTest, CreateDisplayAddress) {
  SetUpDictionary(kClientValidAddress);
  Address address("country_code",
                  ASCIIToUTF16("name"),
                  ASCIIToUTF16("address1"),
                  ASCIIToUTF16("address2"),
                  ASCIIToUTF16("city"),
                  ASCIIToUTF16("state"),
                  ASCIIToUTF16("postal_code"),
                  ASCIIToUTF16("phone_number"),
                  std::string());
  EXPECT_EQ(address, *Address::CreateDisplayAddress(*dict_));
}

TEST_F(WalletAddressTest, ToDictionaryWithoutID) {
  base::DictionaryValue expected;
  expected.SetString("country_name_code",
                     "country_name_code");
  expected.SetString("recipient_name",
                     "recipient_name");
  expected.SetString("locality_name",
                     "locality_name");
  expected.SetString("administrative_area_name",
                     "administrative_area_name");
  expected.SetString("postal_code_number",
                     "postal_code_number");
  base::ListValue* address_lines = new base::ListValue();
  address_lines->AppendString("address_line_1");
  address_lines->AppendString("address_line_2");
  expected.Set("address_line", address_lines);

  Address address("country_name_code",
                  ASCIIToUTF16("recipient_name"),
                  ASCIIToUTF16("address_line_1"),
                  ASCIIToUTF16("address_line_2"),
                  ASCIIToUTF16("locality_name"),
                  ASCIIToUTF16("administrative_area_name"),
                  ASCIIToUTF16("postal_code_number"),
                  ASCIIToUTF16("phone_number"),
                  std::string());

  EXPECT_TRUE(expected.Equals(address.ToDictionaryWithoutID().get()));
}

TEST_F(WalletAddressTest, ToDictionaryWithID) {
  base::DictionaryValue expected;
  expected.SetString("id", "id");
  expected.SetString("phone_number", "phone_number");
  expected.SetString("postal_address.country_name_code",
                     "country_name_code");
  expected.SetString("postal_address.recipient_name",
                     "recipient_name");
  expected.SetString("postal_address.locality_name",
                     "locality_name");
  expected.SetString("postal_address.administrative_area_name",
                     "administrative_area_name");
  expected.SetString("postal_address.postal_code_number",
                     "postal_code_number");
  base::ListValue* address_lines = new base::ListValue();
  address_lines->AppendString("address_line_1");
  address_lines->AppendString("address_line_2");
  expected.Set("postal_address.address_line", address_lines);

  Address address("country_name_code",
                  ASCIIToUTF16("recipient_name"),
                  ASCIIToUTF16("address_line_1"),
                  ASCIIToUTF16("address_line_2"),
                  ASCIIToUTF16("locality_name"),
                  ASCIIToUTF16("administrative_area_name"),
                  ASCIIToUTF16("postal_code_number"),
                  ASCIIToUTF16("phone_number"),
                  "id");

  EXPECT_TRUE(expected.Equals(address.ToDictionaryWithID().get()));
}

TEST_F(WalletAddressTest, FromAutofillProfile) {
  {
    AutofillProfile profile(test::GetFullProfile());
    profile.SetRawInfo(ADDRESS_HOME_STATE, ASCIIToUTF16("tx"));
    Address address(profile);
    EXPECT_EQ(ASCIIToUTF16("TX"), address.administrative_area_name());
  }

  {
    AutofillProfile profile(test::GetFullProfile());
    profile.SetRawInfo(ADDRESS_HOME_STATE, ASCIIToUTF16("Texas"));
    Address address(profile);
    EXPECT_EQ(ASCIIToUTF16("TX"), address.administrative_area_name());
  }

  {
    AutofillProfile profile(test::GetFullProfile());
    profile.SetRawInfo(ADDRESS_HOME_STATE, ASCIIToUTF16("TX"));
    Address address(profile);
    EXPECT_EQ(ASCIIToUTF16("TX"), address.administrative_area_name());
  }

  {
    AutofillProfile profile(test::GetFullProfile());
    profile.SetRawInfo(ADDRESS_HOME_STATE, ASCIIToUTF16("txeas"));
    Address address(profile);
    EXPECT_TRUE(address.administrative_area_name().empty());
  }
}

}  // namespace wallet
}  // namespace autofill