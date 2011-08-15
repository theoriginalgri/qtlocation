#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qgeoplace.h>
#include <qplacemediaobject.h>

QT_USE_NAMESPACE

class tst_QGeoPlace : public QObject
{
    Q_OBJECT

public:
    tst_QGeoPlace();

private Q_SLOTS:
    void constructorTest();
    void additionalDataTest();
    void businessInformationTest();
    void categoriesTest();
    void descriptionsTest();
    void detailsFetchedTest();
    void locationTest();
    void ratingTest();
    void suppliersTest();
    void feedsTest();
    void mediaTest();
    void mediaCountTest();
    void nameTest();
    void placeIdTest();
    void reviewsTest();
    void reviewCountTest();
    void shortDescriptionTest();
    void tagsTest();
    void primaryPhoneTest();
    void primaryFaxTest();
    void primaryEmailTest();
    void primaryUrlTest();
    void operatorsTest();
};

tst_QGeoPlace::tst_QGeoPlace()
{
}

void tst_QGeoPlace::constructorTest()
{
    QGeoPlace testObj;
    testObj.setPlaceId("testId");
    QPlaceBusinessInformation obj;
    obj.setOpeningNote("10");
    testObj.setBusinessInformation(obj);
    QGeoLocation loc;
    loc.setCoordinate(QGeoCoordinate(10,20));
    testObj.setLocation(loc);
    QGeoPlace *testObjPtr = new QGeoPlace(testObj);

    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");

    delete testObjPtr;
}

void tst_QGeoPlace::additionalDataTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.additionalData().count() == 0, "Wrong default value");
    QVariantHash list;
    list.insert("key1", "value1");
    list.insert("key2", "value2");
    testObj.setAdditionalData(list);
    QVERIFY2(testObj.additionalData().count() == 2, "Wrong value returned");
    QVERIFY2(testObj.additionalData()["key1"] == "value1", "Wrong value[1] returned");
}

void tst_QGeoPlace::nameTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.name() == QString(), "Wrong default value");
    testObj.setName("testText");
    QVERIFY2(testObj.name() == "testText", "Wrong value returned");
}

void tst_QGeoPlace::placeIdTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.placeId() == QString(), "Wrong default value");
    testObj.setPlaceId("testText");
    QVERIFY2(testObj.placeId() == "testText", "Wrong value returned");
}

void tst_QGeoPlace::shortDescriptionTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.shortDescription() == QString(), "Wrong default value");
    testObj.setShortDescription("testText");
    QVERIFY2(testObj.shortDescription() == "testText", "Wrong value returned");
}

void tst_QGeoPlace::mediaCountTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.mediaCount(QPlaceMediaObject::Image) == 0, "Wrong default value");
    QVERIFY2(testObj.mediaCount("Video") == 0, "Wrong default value");
    testObj.setMediaCount(QPlaceMediaObject::Image, 50);
    QVERIFY2(testObj.mediaCount(QPlaceMediaObject::Image) == 50, "Wrong value returned");
    QVERIFY2(testObj.mediaCount("Video") == 0, "Wrong value returned");
}

void tst_QGeoPlace::reviewCountTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.reviewCount() == 0, "Wrong default value");
    testObj.setReviewCount(10);
    QVERIFY2(testObj.reviewCount() == 10, "Wrong value returned");
}

void tst_QGeoPlace::ratingTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.rating() == QPlaceRating(), "Wrong default value");
    QPlaceRating obj;
    obj.setCount(10);
    testObj.setRating(obj);
    QVERIFY2(testObj.rating() == obj, "Wrong value returned");
}

void tst_QGeoPlace::locationTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.location() == QGeoLocation(), "Wrong default value");
    QGeoLocation obj;
    obj.setCoordinate(QGeoCoordinate(10,20));
    testObj.setLocation(obj);
    QVERIFY2(testObj.location() == obj, "Wrong value returned");
}

void tst_QGeoPlace::businessInformationTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.businessInformation() == QPlaceBusinessInformation(), "Wrong default value");
    QPlaceBusinessInformation obj;
    obj.setOpeningNote("10");
    testObj.setBusinessInformation(obj);
    QVERIFY2(testObj.businessInformation() == obj, "Wrong value returned");
}

void tst_QGeoPlace::descriptionsTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.descriptions().count() == 0, "Wrong default value");
    QPlaceDescription sup;
    sup.setContent("testId");
    QList<QPlaceDescription> list;
    list.append(sup);
    sup.setContent("testName2");
    list.append(sup);
    testObj.setDescriptions(list);
    QVERIFY2(testObj.descriptions().count() == 2, "Wrong value returned");
}

void tst_QGeoPlace::detailsFetchedTest()
{
    QGeoPlace testPlace;
    QVERIFY2(testPlace.detailsFetched() == false, "Wrong default value");
    testPlace.setDetailsFetched(true);
    QVERIFY2(testPlace.detailsFetched() == true, "Wrong value returned");
    testPlace.setDetailsFetched(false);
    QVERIFY2(testPlace.detailsFetched() == false, "Wrong value returned");
}

void tst_QGeoPlace::mediaTest()
{
    QUrl thumbnailUrl("testId");
    QUrl mediaUrl("testName2");

    QGeoPlace place;
    QVERIFY2(place.media(QPlaceMediaObject::Image).count() ==0,"Wrong default value");
    QVERIFY2(place.media(QPlaceMediaObject::Image).count() ==0,"Wrong default value");
    QVERIFY2(place.media("Video").count() ==0,"Wrong default value");


    QPlaceMediaObject dummyImage;
    dummyImage.setUrl(QUrl("www.dummy.one"));

    QPlaceMediaObject dummyImage2;
    dummyImage2.setUrl(QUrl("www.dummy.two"));

    QPlaceMediaObject dummyImage3;
    dummyImage3.setUrl(QUrl("www.dummy.three"));

    PlaceMediaCollection imageCollection;
    imageCollection.insert(0,dummyImage);
    imageCollection.insert(1, dummyImage2);
    imageCollection.insert(2, dummyImage3);

    place.setMedia(QPlaceMediaObject::Image, imageCollection);
    PlaceMediaCollection retrievedCollection = place.media(QPlaceMediaObject::Image);

    QCOMPARE(retrievedCollection.count(), 3);
    QCOMPARE(retrievedCollection.value(0), dummyImage);
    QCOMPARE(retrievedCollection.value(1), dummyImage2);
    QCOMPARE(retrievedCollection.value(2), dummyImage3);
    QCOMPARE(place.media("Video").count(), 0);


    //replace the second and insert a sixth image
    //indexes 4 and 5 are "missing"
    QPlaceMediaObject dummyImage2New;
    dummyImage2.setUrl(QUrl("www.dummy.two.new"));

    QPlaceMediaObject dummyImage6;
    dummyImage6.setUrl(QUrl("www.dummy.six"));

    imageCollection.clear();
    imageCollection.insert(1, dummyImage2New);
    imageCollection.insert(5, dummyImage6);
    place.addMedia(QPlaceMediaObject::Image, imageCollection);

    retrievedCollection = place.media(QPlaceMediaObject::Image);
    QCOMPARE(retrievedCollection.count(), 4);
    QCOMPARE(retrievedCollection.value(0), dummyImage);
    QCOMPARE(retrievedCollection.value(1), dummyImage2New);
    QCOMPARE(retrievedCollection.value(2), dummyImage3);
    QCOMPARE(retrievedCollection.value(3), QPlaceMediaObject());
    QCOMPARE(retrievedCollection.value(4), QPlaceMediaObject());
    QCOMPARE(retrievedCollection.value(5), dummyImage6);
    QCOMPARE(place.media("Video").count(), 0);
}

void tst_QGeoPlace::reviewsTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.reviews().items() == 0, "Wrong default value");
    QPlaceReview sup;
    sup.setDescription("testId");
    QList<QPlaceReview> list;
    list.append(sup);
    sup.setLanguage("testName2");
    list.append(sup);
    QPlacePaginationList<QPlaceReview> reviewsList;
    reviewsList.setData(list);
    testObj.setReviews(reviewsList);
    QVERIFY2(testObj.reviews().items() == 2, "Wrong value returned");
    QVERIFY2(testObj.reviews().data()[1].language() == "testName2", "Wrong value returned");
    QVERIFY2(testObj.reviews().start() == 0, "Wrong value returned");
    QVERIFY2(testObj.reviews().stop() == 1, "Wrong value returned");
}

void tst_QGeoPlace::categoriesTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.categories().count() == 0, "Wrong default value");
    QPlaceCategory sup;
    sup.setCategoryId("testId");
    QList<QPlaceCategory> list;
    list.append(sup);
    sup.setCategoryId("testName2");
    list.append(sup);
    testObj.setCategories(list);
    QVERIFY2(testObj.categories().count() == 2, "Wrong value returned");
}

void tst_QGeoPlace::suppliersTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.suppliers().count() == 0, "Wrong default value");
    QPlaceSupplier sup;
    sup.setName("testName1");
    sup.setSupplierId("testId");
    QList<QPlaceSupplier> list;
    list.append(sup);
    sup.setName("testName2");
    list.append(sup);
    testObj.setSuppliers(list);
    QVERIFY2(testObj.suppliers().count() == 2, "Wrong value returned");
}

void tst_QGeoPlace::feedsTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.feeds().count() == 0, "Wrong default value");
    QStringList list;
    list << "1" << "2" << "3";
    testObj.setFeeds(list);
    QVERIFY2(testObj.feeds().count() == 3, "Wrong value returned");
    QVERIFY2(testObj.feeds()[1] == "2", "Wrong [1] value returned");
}

void tst_QGeoPlace::tagsTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.tags().count() == 0, "Wrong default value");
    QStringList list;
    list << "1" << "2" << "3";
    testObj.setTags(list);
    QVERIFY2(testObj.tags().count() == 3, "Wrong value returned");
    QVERIFY2(testObj.tags()[1] == "2", "Wrong [1] value returned");
}

void tst_QGeoPlace::primaryPhoneTest()
{
    QGeoPlace place;
    QVERIFY2(place.primaryPhone().isEmpty(), "Wrong default value");

    place.setPrimaryPhone("555-5555");
    QCOMPARE(place.primaryPhone(), QString("555-5555"));

    place.setPrimaryPhone(QString());
    QCOMPARE(place.primaryPhone(), QString());
}

void tst_QGeoPlace::primaryFaxTest()
{
    QGeoPlace place;
    QVERIFY2(place.primaryFax().isEmpty(), "Wrong default value");

    place.setPrimaryFax("555-5555");
    QCOMPARE(place.primaryFax(), QString("555-5555"));

    place.setPrimaryFax(QString());
    QCOMPARE(place.primaryFax(), QString());
}

void tst_QGeoPlace::primaryEmailTest()
{
    QGeoPlace place;
    QVERIFY2(place.primaryEmail().isEmpty(), "Wrong default value");

    place.setPrimaryEmail("test@test.com");
    QCOMPARE(place.primaryEmail(), QString("test@test.com"));


    place.setPrimaryEmail(QString());
    QCOMPARE(place.primaryEmail(), QString());
}

void tst_QGeoPlace::primaryUrlTest()
{
    QGeoPlace place;
    QVERIFY2(place.primaryUrl().isEmpty(), "Wron default value");

    place.setPrimaryUrl(QUrl("www.winterfell.com"));
    QCOMPARE(place.primaryUrl(), QUrl("www.winterfell.com"));

    place.setPrimaryUrl(QUrl());
    QCOMPARE(place.primaryUrl(), QUrl());
}

void tst_QGeoPlace::operatorsTest()
{
    QGeoPlace testObj;
    testObj.setPlaceId("testId");
    QPlaceBusinessInformation obj;
    obj.setOpeningNote("10");
    testObj.setBusinessInformation(obj);
    QStringList tags;
    tags << "1" << "2" << "3";
    testObj.setTags(tags);
    QGeoLocation loc;
    loc.setCoordinate(QGeoCoordinate(10,20));
    testObj.setLocation(loc);

    QGeoPlace testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setPlaceId("342-456");
    QVERIFY2(testObj != testObj2, "Object should be different");
}

QTEST_APPLESS_MAIN(tst_QGeoPlace);

#include "tst_qgeoplace.moc"