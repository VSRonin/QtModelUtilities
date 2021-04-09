// This example will demonstrate how to use RoleMaskProxyModel to leverage roles not supported by the source model.
// In the specific we will use Qt::BackgroundRole on a QStringListModel to highlight all elements that contain a certain string

#include <RoleMaskProxyModel>
#include <QStringListModel>
#include <QListView>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // Create the source model, in this case a list of countries
    QStringListModel baseModel(QStringList({"Afghanistan",
                                            "Albania",
                                            "Algeria",
                                            "Andorra",
                                            "Angola",
                                            "Anguilla",
                                            "Antigua & Barbuda",
                                            "Argentina",
                                            "Armenia",
                                            "Australia",
                                            "Austria",
                                            "Azerbaijan",
                                            "Bahamas",
                                            "Bahrain",
                                            "Bangladesh",
                                            "Barbados",
                                            "Belarus",
                                            "Belgium",
                                            "Belize",
                                            "Benin",
                                            "Bermuda",
                                            "Bhutan",
                                            "Bolivia",
                                            "Bosnia & Herzegovina",
                                            "Botswana",
                                            "Brazil",
                                            "Brunei Darussalam",
                                            "Bulgaria",
                                            "Burkina Faso",
                                            "Myanmar/Burma",
                                            "Burundi",
                                            "Cambodia",
                                            "Cameroon",
                                            "Canada",
                                            "Cape Verde",
                                            "Cayman Islands",
                                            "Central African Republic",
                                            "Chad",
                                            "Chile",
                                            "China",
                                            "Colombia",
                                            "Comoros",
                                            "Congo",
                                            "Costa Rica",
                                            "Croatia",
                                            "Cuba",
                                            "Cyprus",
                                            "Czech Republic",
                                            "Democratic Republic of the Congo",
                                            "Denmark",
                                            "Djibouti",
                                            "Dominica",
                                            "Dominican Republic",
                                            "Ecuador",
                                            "Egypt",
                                            "El Salvador",
                                            "Equatorial Guinea",
                                            "Eritrea",
                                            "Estonia",
                                            "Ethiopia",
                                            "Fiji",
                                            "Finland",
                                            "France",
                                            "French Guiana",
                                            "Gabon",
                                            "Gambia",
                                            "Georgia",
                                            "Germany",
                                            "Ghana",
                                            "Great Britain",
                                            "Greece",
                                            "Grenada",
                                            "Guadeloupe",
                                            "Guatemala",
                                            "Guinea",
                                            "Guinea-Bissau",
                                            "Guyana",
                                            "Haiti",
                                            "Honduras",
                                            "Hungary",
                                            "Iceland",
                                            "India",
                                            "Indonesia",
                                            "Iran",
                                            "Iraq",
                                            "Israel and the Occupied Territories",
                                            "Italy",
                                            "Ivory Coast (Cote d'Ivoire)",
                                            "Jamaica",
                                            "Japan",
                                            "Jordan",
                                            "Kazakhstan",
                                            "Kenya",
                                            "Kosovo",
                                            "Kuwait",
                                            "Kyrgyz Republic (Kyrgyzstan)",
                                            "Laos",
                                            "Latvia",
                                            "Lebanon",
                                            "Lesotho",
                                            "Liberia",
                                            "Libya",
                                            "Liechtenstein",
                                            "Lithuania",
                                            "Luxembourg",
                                            "Republic of Macedonia",
                                            "Madagascar",
                                            "Malawi",
                                            "Malaysia",
                                            "Maldives",
                                            "Mali",
                                            "Malta",
                                            "Martinique",
                                            "Mauritania",
                                            "Mauritius",
                                            "Mayotte",
                                            "Mexico",
                                            "Moldova, Republic of",
                                            "Monaco",
                                            "Mongolia",
                                            "Montenegro",
                                            "Montserrat",
                                            "Morocco",
                                            "Mozambique",
                                            "Namibia",
                                            "Nepal",
                                            "Netherlands",
                                            "New Zealand",
                                            "Nicaragua",
                                            "Niger",
                                            "Nigeria",
                                            "Korea, Democratic Republic of (North Korea)",
                                            "Norway",
                                            "Oman",
                                            "Pacific Islands",
                                            "Pakistan",
                                            "Panama",
                                            "Papua New Guinea",
                                            "Paraguay",
                                            "Peru",
                                            "Philippines",
                                            "Poland",
                                            "Portugal",
                                            "Puerto Rico",
                                            "Qatar",
                                            "Reunion",
                                            "Romania",
                                            "Russian Federation",
                                            "Rwanda",
                                            "Saint Kitts and Nevis",
                                            "Saint Lucia",
                                            "Saint Vincent's & Grenadines",
                                            "Samoa",
                                            "Sao Tome and Principe",
                                            "Saudi Arabia",
                                            "Senegal",
                                            "Serbia",
                                            "Seychelles",
                                            "Sierra Leone",
                                            "Singapore",
                                            "Slovak Republic (Slovakia)",
                                            "Slovenia",
                                            "Solomon Islands",
                                            "Somalia",
                                            "South Africa",
                                            "Korea, Republic of (South Korea)",
                                            "South Sudan",
                                            "Spain",
                                            "Sri Lanka",
                                            "Sudan",
                                            "Suriname",
                                            "Swaziland",
                                            "Sweden",
                                            "Switzerland",
                                            "Syria",
                                            "Tajikistan",
                                            "Tanzania",
                                            "Thailand",
                                            "Timor Leste",
                                            "Togo",
                                            "Trinidad & Tobago",
                                            "Tunisia",
                                            "Turkey",
                                            "Turkmenistan",
                                            "Turks & Caicos Islands",
                                            "Uganda",
                                            "Ukraine",
                                            "United Arab Emirates",
                                            "United States of America (USA)",
                                            "Uruguay",
                                            "Uzbekistan",
                                            "Venezuela",
                                            "Vietnam",
                                            "Virgin Islands (UK)",
                                            "Virgin Islands (US)",
                                            "Yemen",
                                            "Zambia",
                                            "Zimbabwe"}));

    // create the proxy model
    RoleMaskProxyModel highlightProxy;
    // set the source model
    highlightProxy.setSourceModel(&baseModel);
    // we will intercept all data in Qt::BackgroundRole and handle it in the proxy rather than passing it on to the source model
    highlightProxy.addMaskedRole(Qt::BackgroundRole);
    // we create a display the ui
    // it will contain a list view with the contents of our proxy model and a line edit
    QWidget mainWid;
    mainWid.setWindowTitle(QStringLiteral("RoleMaskProxyModel Example - Highlighted QStringListModel"));
    QLineEdit *highlighterEdit = new QLineEdit(&mainWid);
    QListView *mainView = new QListView(&mainWid);
    mainView->setModel(&highlightProxy);
    QVBoxLayout *mainLay = new QVBoxLayout(&mainWid);
    mainLay->addWidget(new QLabel(QStringLiteral("Highlight cells containing:")));
    mainLay->addWidget(highlighterEdit);
    mainLay->addWidget(mainView);
    mainWid.show();

    // we implement the highlighting functionality
    QObject::connect(highlighterEdit, &QLineEdit::textEdited, [&highlightProxy](const QString &filterString) {
        // every time the lineedit text changes
        for (int i = 0; i < highlightProxy.rowCount(); ++i) {
            // iterate over all elements of the model
            const QModelIndex currentIdx = highlightProxy.index(i, 0);
            // if the element contains the input string
            if (!filterString.isEmpty() && currentIdx.data().toString().contains(filterString, Qt::CaseInsensitive)) {
                // highlight it
                highlightProxy.setData(currentIdx, QColor(Qt::yellow), Qt::BackgroundRole);
            } else {
                // otherwise reset the background color
                highlightProxy.setData(currentIdx, QVariant(), Qt::BackgroundRole);
            }
        }
    });

    return app.exec();
}