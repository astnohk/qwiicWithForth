\
\ Read value from HTU21D via I2C
\
\ Use:
\     include htu21d.fth
\     htu21d-temp F.
\     htu21d-humid F.
\

: htu21d-temp
    $f3 1 $40 i2cwrite drop
    200 ms
    3 $40 i2cread drop drop
    swap 256 * +
    S>F 65536.0 F/ 175.72 F* 46.85 F- ;

: htu21d-humid
    $f5 1 $40 i2cwrite drop
    200 ms
    3 $40 i2cread drop drop
    swap 256 * +
    S>F 65536.0 F/ 125.0 F* 6.0 F- ;

