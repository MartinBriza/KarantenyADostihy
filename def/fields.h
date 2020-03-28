::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "START"), 0, {
    ::Effect { ::Effect::PLAYER, ::Effect::GAIN, 4000 }
}, QColor("white"), 0, ::Field::BASIC),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Fantome"), 1200, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 40 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 200 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 600 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 1800 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 3200 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 5000 },
}, QColor("orange"), 1000, ::Field::HORSE),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Finance"), 0, {
    ::Effect { ::Effect::PLAYER, ::Effect::DRAW_FINANCE }
}, QColor("white"), 0, ::Field::DECK),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Gavora"), 1200, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 40 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 200 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 600 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 1800 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 3200 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 5000 },
}, QColor("orange"), 1000, ::Field::HORSE),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Veterinary checkup"), 0, {
    ::Effect { ::Effect::PLAYER, ::Effect::FEE, 500 }
}, QColor("white"), 0, ::Field::BASIC),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "1. trainer"), 4000, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 1000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 2000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 3000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 4000 },
}, QColor("white"), 0, ::Field::TRAINER),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Lady Anne"), 2000, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 120 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 600 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 1800 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 5400 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 8000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 11000 },
}, QColor("brown"), 1000, ::Field::HORSE),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Chance"), 0, {
    ::Effect { ::Effect::PLAYER, ::Effect::DRAW_CHANCE }
}, QColor("white"), 0, ::Field::DECK),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Pasek"), 2000, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 120 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 600 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 1200 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 5400 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 8000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 11000 },
}, QColor("brown"), 1000, ::Field::HORSE),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Koran"), 2400, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 160 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 800 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 2000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 6000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 9000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 12000 },
}, QColor("brown"), 1000, ::Field::HORSE),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Suspension"), 0, {
    ::Effect { ::Effect::PLAYER, ::Effect::WAIT, -1 }
}, QColor("white"), 0, ::Field::BASIC),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Neklan"), 2800, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 200 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 1000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 3000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 9000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 12500 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 15000 },
}, QColor("lightblue"), 2000, ::Field::HORSE),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Transport"), 3000, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 80 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 200 },
}, QColor("white"), 0, ::Field::TRANSPORT),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Portlancl"), 2800, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 200 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 1000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 3000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 9000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 12500 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 15000 },
}, QColor("lightblue"), 2000, ::Field::HORSE),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Japan"), 2800, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 240 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 1200 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 3600 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 10000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 14000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 18000 },
}, QColor("lightblue"), 2000, ::Field::HORSE),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "2. trainer"), 4000, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 1000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 2000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 3000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 4000 },
}, QColor("white"), 0, ::Field::TRAINER),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Kostrava"), 3600, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 280 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 1400 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 4000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 11000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 15000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 19000 },
}, QColor("greenyellow"), 2000, ::Field::HORSE),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Finance"), 0, {
    ::Effect { ::Effect::PLAYER, ::Effect::DRAW_FINANCE }
}, QColor("white"), 0, ::Field::DECK),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Lukava"), 3600, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 280 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 1400 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 4000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 11000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 15000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 19000 },
}, QColor("greenyellow"), 2000, ::Field::HORSE),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Melák"), 4000, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 320 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 1600 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 4400 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 12000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 16000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 20000 },
}, QColor("greenyellow"), 2000, ::Field::HORSE),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Parkoviště"), 0, {
}, QColor("white"), 0, ::Field::BASIC),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Grifel"), 4400, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 360 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 1800 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 5000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 14000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 17000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 21000 },
}, QColor("red"), 3000, ::Field::HORSE),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Chance"), 0, {
    ::Effect { ::Effect::PLAYER, ::Effect::DRAW_CHANCE }
}, QColor("white"), 0, ::Field::DECK),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Mohyla"), 4400, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 360 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 1800 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 5000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 14000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 17000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 21000 },
}, QColor("red"), 3000, ::Field::HORSE),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Metál"), 4800, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 400 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 2000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 6000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 15000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 18000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 22000 },
}, QColor("red"), 3000, ::Field::HORSE),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "3. trainer"), 4000, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 1000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 2000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 3000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 4000 },
}, QColor("white"), 0, ::Field::TRAINER),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Tara"), 5200, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 440 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 2200 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 6600 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 16000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 19500 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 23000 },
}, QColor("yellow"), 3000, ::Field::HORSE),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Furioso"), 5200, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 440 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 2200 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 6600 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 16000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 19500 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 23000 },
}, QColor("yellow"), 3000, ::Field::HORSE),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Stables"), 3000, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 80 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 200 },
}, QColor("white"), 0, ::Field::TRANSPORT),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Genius"), 5600, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 580 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 2400 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 7200 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 17000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 20500 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 24000 },
}, QColor("yellow"), 3000, ::Field::HORSE),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Suspected from doping "), 0, {
    ::Effect { ::Effect::PLAYER, ::Effect::WAIT, 1 },
}, QColor("white"), 0, ::Field::BASIC),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Shagga"), 6000, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 500 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 2600 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 7800 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 18000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 22000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 25500 },
}, QColor("green"), 4000, ::Field::HORSE),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Dahoman"), 6000, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 500 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 2600 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 7800 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 18000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 22000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 25500 },
}, QColor("green"), 4000, ::Field::HORSE),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Finance"), 0, {
    ::Effect { ::Effect::PLAYER, ::Effect::DRAW_FINANCE }
}, QColor("white"), 0, ::Field::DECK),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Gira"), 6400, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 560 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 3000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 9000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 20000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 24000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 28000 },
}, QColor("green"), 4000, ::Field::HORSE),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "4. trainer"), 4000, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 1000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 2000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 3000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 4000 },
}, QColor("white"), 0, ::Field::TRAINER),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Chance"), 0, {
    ::Effect { ::Effect::PLAYER, ::Effect::DRAW_CHANCE }
}, QColor("white"), 0, ::Field::DECK),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Narcius"), 7000, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 700 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 3500 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 10000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 22000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 26000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 30000 },
}, QColor("slateblue"), 4000, ::Field::HORSE),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Veterinary checkup"), 0,  {
    ::Effect { ::Effect::PLAYER, ::Effect::FEE, 500 }
}, QColor("white"), 0, ::Field::BASIC),

::Field(lastCardID++, QT_TRANSLATE_NOOP("Fields", "Napoli"), 8000, {
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 1000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 4000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 12000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 28000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 34000 },
    ::Effect { ::Effect::PLAYER, ::Effect::TRANSFER, 40000 },
}, QColor("slateblue"), 4000, ::Field::HORSE)
