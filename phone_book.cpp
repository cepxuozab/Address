#include "phone_book.h"

#include <contact.pb.h>

#include <algorithm>

using namespace std;

PhoneBook::PhoneBook(vector<Contact> contacts)
    : contacts_(move(contacts)) {
    sort(contacts_.begin(), contacts_.end(), [](const Contact& l, const Contact& r) {
        return l.name < r.name;
    });
}

PhoneBook::ContactRange PhoneBook::FindByNamePrefix(string_view name_prefix) const {
    auto range_l
        = lower_bound(contacts_.begin(), contacts_.end(), name_prefix, [](const Contact& l, string_view r) {
              return l.name < r;
          });

    auto range_r
        = upper_bound(contacts_.begin(), contacts_.end(), name_prefix, [](string_view l, const Contact& r) {
              string_view prefix = string_view(r.name).substr(0, l.size());
              return l < prefix;
          });
    return {range_l, range_r};
}

void PhoneBook::SaveTo(ostream& output) const {
    phone_book_serialize::ContactList contact_list;

    for (const Contact& c : contacts_) {
        phone_book_serialize::Contact& new_contact = *contact_list.add_contact();
        new_contact.set_name(c.name);

        if (c.birthday) {
            new_contact.mutable_birthday()->set_day(c.birthday->day);
            new_contact.mutable_birthday()->set_month(c.birthday->month);
            new_contact.mutable_birthday()->set_year(c.birthday->year);
        }

        for (const string& phone : c.phones) {
            new_contact.add_phone_number(phone);
        }
    }

    contact_list.SerializeToOstream(&output);
}

PhoneBook DeserializePhoneBook(istream& input) {
    phone_book_serialize::ContactList contact_list;
    if (!contact_list.ParseFromIstream(&input)) {
        return PhoneBook({});
    }

    vector<Contact> result_vector;
    result_vector.reserve(contact_list.contact_size());

    for (const auto& contact : contact_list.contact()) {
        Contact c;
        c.name = contact.name();
        if (contact.has_birthday()) {
            c.birthday = Date{contact.birthday().year(), contact.birthday().month(), contact.birthday().day()};
        }

        c.phones.reserve(contact.phone_number_size());
        for (const string& phone : contact.phone_number()) {
            c.phones.push_back(phone);
        }

        result_vector.push_back(move(c));
    }

    return PhoneBook(move(result_vector));
}