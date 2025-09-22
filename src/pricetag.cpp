#include "pricetag.h"


PriceTag::PriceTag () : price (0.0), quantity (0), price2 (0.0) {}

PriceTag::PriceTag (const QString &name, const QString &description, double price, int quantity) :
    name (name), description (description), price (price), quantity (quantity), price2 (0.0)
{}


QString PriceTag::getName () const { return name; }

void PriceTag::setName (const QString &name) { this->name = name; }

QString PriceTag::getDescription () const { return description; }

void PriceTag::setDescription (const QString &description) { this->description = description; }

double PriceTag::getPrice () const { return price; }

void PriceTag::setPrice (double price) { this->price = price; }

int PriceTag::getQuantity () const { return quantity; }

void PriceTag::setQuantity (int quantity) { this->quantity = quantity; }


QString PriceTag::getSupplier () const { return supplier; }

void PriceTag::setSupplier (const QString &supplier) { this->supplier = supplier; }

QString PriceTag::getAddress () const { return address; }

void PriceTag::setAddress (const QString &address) { this->address = address; }

QString PriceTag::getBrand () const { return brand; }

void PriceTag::setBrand (const QString &brand) { this->brand = brand; }

QString PriceTag::getCategory () const { return category; }

void PriceTag::setCategory (const QString &category) { this->category = category; }

QString PriceTag::getAdditionalData () const { return additionalData; }

void PriceTag::setAdditionalData (const QString &data) { this->additionalData = data; }

QString PriceTag::getGender () const { return gender; }

void PriceTag::setGender (const QString &gender) { this->gender = gender; }

QString PriceTag::getBrandCountry () const { return brandCountry; }

void PriceTag::setBrandCountry (const QString &country) { this->brandCountry = country; }

QString PriceTag::getManufacturingPlace () const { return manufacturingPlace; }

void PriceTag::setManufacturingPlace (const QString &place) { this->manufacturingPlace = place; }

QString PriceTag::getMaterial () const { return material; }

void PriceTag::setMaterial (const QString &material) { this->material = material; }

QString PriceTag::getSize () const { return size; }

void PriceTag::setSize (const QString &size) { this->size = size; }

QString PriceTag::getArticle () const { return article; }

void PriceTag::setArticle (const QString &article) { this->article = article; }

double PriceTag::getPrice2 () const { return price2; }

void PriceTag::setPrice2 (double price) { this->price2 = price; }

QString PriceTag::getAdditionalData2 () const { return additionalData2; }

void PriceTag::setAdditionalData2 (const QString &data) { this->additionalData2 = data; }


// Service methods:
QString PriceTag::getFormattedCategory () const
{
    if (category.isEmpty ())
        return QString ();


    if (category.length () <= 12 && ! gender.isEmpty ())
        return category + " " + gender;


    return category;
}

bool PriceTag::hasDiscount () const { return price2 > 0.0 && price2 < price; }

double PriceTag::getDiscountPrice () const { return hasDiscount () ? price2 : price; }

double PriceTag::getOriginalPrice () const { return hasDiscount () ? price : price2; }
