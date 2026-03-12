: ds3231-parse
    dup 4 >> 10 *
    swap
    $0f and
    + ;

: ds3231-read-date
    0 1 7 $68 i2creadreg drop
    ds3231-parse 2000 +
    1 roll ds3231-parse
    2 roll ds3231-parse
    3 roll ds3231-parse
    4 roll ds3231-parse
    5 roll ds3231-parse
    7 roll ds3231-parse ;

