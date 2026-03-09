#ifndef PF_USER_CUSTOM

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#include "pforth/csrc/pf_all.h"

#define I2C_BUFFER_LENGTH_MAX 256

static cell_t I2CREAD(cell_t buffer_len, cell_t addr);
static cell_t I2CWRITE(cell_t buffer_len, cell_t addr);
static cell_t I2CREADREG(cell_t w_len, cell_t r_len, cell_t addr);

static cell_t I2CREAD(cell_t buffer_len, cell_t addr)
{
    const char *dev = "/dev/i2c-1";
    struct i2c_msg msg;
    struct i2c_rdwr_ioctl_data packets;
    int8_t ret;
    uint8_t status = 0;
    int32_t fd;
    uint8_t buffer[I2C_BUFFER_LENGTH_MAX];
    size_t buf_len;
    size_t i;

    MSG("I2CREAD: ADDR = ");
    ffDot(addr);
    MSG(", buf_len = ");
    ffDot(buffer_len);

    buf_len = (size_t)buffer_len;
    if (buf_len > I2C_BUFFER_LENGTH_MAX)
    {
        MSG("I2CREAD: buf_len exceeds ");
        ffDot(I2C_BUFFER_LENGTH_MAX);
        return -1;
    }

    msg.addr = addr;
    msg.flags = I2C_M_RD;
    msg.buf = buffer;
    msg.len = buf_len;

    packets.msgs = &msg;
    packets.nmsgs = 1;

    fd = open(dev, O_RDWR);
    if (fd < 0)
    {
        perror(dev);
        status = 255;
        return status;
    }
    ret = ioctl(fd, I2C_RDWR, &packets);
    if (ret < 0)
    {
        perror(dev);
        status = -1;
        return status;
    }
    close(fd);

    MSG("I2CREAD: success to read.");

    for (i = 0; i < buf_len; i++)
    {
        PUSH_DATA_STACK((cell_t)buffer[i]);
    }
    return 0;
}

static cell_t I2CWRITE(cell_t buffer_len, cell_t addr)
{
    const char *dev = "/dev/i2c-1";
    struct i2c_msg msg;
    struct i2c_rdwr_ioctl_data packets;
    int8_t ret;
    uint8_t status = 0;
    int32_t fd;
    uint8_t buffer[I2C_BUFFER_LENGTH_MAX];
    size_t buf_len;
    size_t i;

    buf_len = (size_t)buffer_len;
    if (buf_len > I2C_BUFFER_LENGTH_MAX)
    {
        MSG("I2CWRITE: buf_len exceeds ");
        ffDot(I2C_BUFFER_LENGTH_MAX);
        return -1;
    }
    for (i = 0; i < buf_len; i++)
    {
        buffer[buf_len - 1 - i] = (uint8_t)POP_DATA_STACK;
    }
    msg.addr = addr;
    msg.flags = 0;
    msg.buf = buffer;
    msg.len = buf_len;

    packets.msgs = &msg;
    packets.nmsgs = 1;

    fd = open(dev, O_RDWR);
    if (fd < 0)
    {
        //perror(dev);
        status = 255;
        return status;
    }
    ret = ioctl(fd, I2C_RDWR, &packets);
    if (ret < 0)
    {
        //perror(dev);
        status = -1;
        return status;
    }

    close(fd);

    return status;
}

static cell_t I2CREADREG(cell_t w_len, cell_t r_len, cell_t addr)
{
    const char *dev = "/dev/i2c-1";
    struct i2c_msg msgs[2];
    struct i2c_rdwr_ioctl_data packets;
    int8_t ret;
    uint8_t status = 0;
    int32_t fd;
    uint8_t wbuffer[I2C_BUFFER_LENGTH_MAX];
    size_t wbuf_len;
    uint8_t rbuffer[I2C_BUFFER_LENGTH_MAX];
    size_t rbuf_len;
    size_t i;

    // Get and Check buffer lengths
    wbuf_len = w_len;
    if (wbuf_len > I2C_BUFFER_LENGTH_MAX)
    {
        MSG("I2CREADREG: wbuf_len exceeds ");
        ffDot(I2C_BUFFER_LENGTH_MAX);
        return -1;
    }
    rbuf_len = r_len;
    if (rbuf_len > I2C_BUFFER_LENGTH_MAX)
    {
        MSG("I2CREADREG: rbuf_len exceeds ");
        ffDot(I2C_BUFFER_LENGTH_MAX);
        return -1;
    }
    // Read write buffer from stack
    for (i = 0; i < wbuf_len; i++)
    {
        wbuffer[wbuf_len - 1 - i] = (uint8_t)POP_DATA_STACK;
    }
    // Create msgs
    msgs[0].addr = addr;
    msgs[0].flags = 0;
    msgs[0].buf = wbuffer;
    msgs[0].len = wbuf_len;
    msgs[1].addr = addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].buf = rbuffer;
    msgs[1].len = rbuf_len;

    packets.msgs = msgs;
    packets.nmsgs = 2;

    fd = open(dev, O_RDWR);
    if (fd < 0)
    {
        perror(dev);
        status = 255;
        return status;
    }
    ret = ioctl(fd, I2C_RDWR, &packets);
    if (ret < 0)
    {
        perror(dev);
        status = -1;
        return status;
    }

    close(fd);

    for (i = 0; i < rbuf_len; i++)
    {
        PUSH_DATA_STACK((cell_t)rbuffer[i]);
    }

    return status;
}

#ifdef PF_NO_GLOBAL_INIT
/******************
** If your loader does not support global initialization, then you
** must define PF_NO_GLOBAL_INIT and provide a function to fill
** the table. Some embedded system loaders require this!
** Do not change the name of LoadCustomFunctionTable()!
** It is called by the pForth kernel.
*/
#define NUM_CUSTOM_FUNCTIONS  (2)
CFunc0 CustomFunctionTable[NUM_CUSTOM_FUNCTIONS];

Err LoadCustomFunctionTable( void )
{
    CustomFunctionTable[0] = I2CREAD;
    CustomFunctionTable[1] = I2CWRITE;
    CustomFunctionTable[2] = I2CREADREG;
    return 0;
}

#else
/******************
** If your loader supports global initialization (most do.) then just
** create the table like this.
*/
CFunc0 CustomFunctionTable[] =
{
    (CFunc0) I2CREAD,
    (CFunc0) I2CWRITE,
    (CFunc0) I2CREADREG
};
#endif

#if (!defined(PF_NO_INIT)) && (!defined(PF_NO_SHELL))
Err CompileCustomFunctions( void )
{
    Err err;
    int i = 0;
/* Compile Forth words that call your custom functions.
** Make sure order of functions matches that in LoadCustomFunctionTable().
** Parameters are: Name in UPPER CASE, Function, Index, Mode, NumParams
*/
    err = CreateGlueToC( "I2CREAD", i++, C_RETURNS_VALUE, 2 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "I2CWRITE", i++, C_RETURNS_VALUE, 2 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "I2CREADREG", i++, C_RETURNS_VALUE, 3 );
    if( err < 0 ) return err;

    return 0;
}
#else
Err CompileCustomFunctions( void ) { return 0; }
#endif

#endif  /* PF_USER_CUSTOM */

