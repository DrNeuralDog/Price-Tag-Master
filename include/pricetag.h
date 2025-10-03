#pragma once

#include "Common.h"


class PriceTag
{
public:
    PriceTag ();
    PriceTag (const QString &name, const QString &description, double price, int quantity);

    // Filling main fields:
    QString getName () const;
    void setName (const QString &name);

    QString getDescription () const;
    void setDescription (const QString &description);

    double getPrice () const;
    void setPrice (double price);

    int getQuantity () const;
    void setQuantity (int quantity);

    QString getSupplier () const;
    void setSupplier (const QString &supplier);

    QString getAddress () const;
    void setAddress (const QString &address);

    QString getBrand () const;
    void setBrand (const QString &brand);

    QString getCategory () const;
    void setCategory (const QString &category);

    QString getAdditionalData () const;
    void setAdditionalData (const QString &data);

    QString getGender () const;
    void setGender (const QString &gender);

    QString getBrandCountry () const;
    void setBrandCountry (const QString &country);

    QString getManufacturingPlace () const;
    void setManufacturingPlace (const QString &place);

    QString getMaterial () const;
    void setMaterial (const QString &material);

    QString getSize () const;
    void setSize (const QString &size);

    QString getArticle () const;
    void setArticle (const QString &article);

    double getPrice2 () const;
    void setPrice2 (double price);

    QString getAdditionalData2 () const;
    void setAdditionalData2 (const QString &data);


    // Service methods:
    QString getFormattedCategory () const;
    bool hasDiscount () const;
    double getDiscountPrice () const;
    double getOriginalPrice () const;


private:
    QString name;
    QString description;
    double price;
    int quantity;
    QString supplier;
    QString address;
    QString brand;
    QString category;
    QString additionalData;
    QString gender;
    QString brandCountry;
    QString manufacturingPlace;
    QString material;
    QString size;
    QString article;
    double price2;
    QString additionalData2;
};
