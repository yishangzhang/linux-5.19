#include <linux/virtio.h>
#include <linux/virtio_ids.h>
#include <linux/virtio_config.h>
#include <linux/module.h>
#include <linux/tpm.h>

#include "tpm.h"

#define VIRTIO_ID_TPM 41

#define TPM_RESPONSE_LEN 512


//copy from tpm_tis_core.h:31
enum virtio_tpm_status {
	TPM_STS_VALID = 0x80,
	TPM_STS_COMMAND_READY = 0x40,
	TPM_STS_GO = 0x20,
	TPM_STS_DATA_AVAIL = 0x10,
	TPM_STS_DATA_EXPECT = 0x08,
	TPM_STS_RESPONSE_RETRY = 0x02,
	TPM_STS_READ_ZERO = 0x23, /* bits that must be zero on read */
};



/* 假设的 _tpm_header_test 结构体定义 */
struct _tpm_header_test {
    __u32 tag;
    __u32 total_size;
    __u32 data_size;
    char data[0]; // 柔性数组，用于存储实际的数据
};

struct virtio_tpm_info;
/* device private data (one per device) */
struct virtio_tpm_dev {
    struct virtqueue *vq;
    struct _tpm_header_test *header;
    struct virtio_tpm_info *priv;
    struct tpm_chip chip;
};

struct virtio_tpm_info{
    struct tpm_chip *chip;
    struct virtio_tpm_dev *vtdev;
    u32 version;
    uint8_t response[512];
    size_t response_len;
};


/* 发送命令的函数 */
static int send_command(struct virtio_tpm_info *vti, u8  *str1,size_t len)
{
    struct virtio_tpm_dev *dev = vti->vtdev;
    //struct virtio_tpm_info *vi= vdev->priv;
    struct scatterlist sg1,sg2,*sgs[2]; // 
    char *request_buffer;
    unsigned int request_len= len;

    /* Allocate request_buffer for the strings */
    request_buffer = kmalloc(request_len, GFP_ATOMIC);
    //request_buffer[1] = kmalloc(request_len[1], GFP_ATOMIC);
    if (!request_buffer ) {
        kfree(request_buffer);
        return ENODEV;
    }

    /* Copy strings to the request_buffer */
    memcpy(request_buffer, str1, request_len);
    //memcpy(request_buffer[1], str2, strlen(str2));

    /* Initialize scatterlist entries */
    sg_init_one(&sg1, request_buffer, request_len);
    sg_init_one(&sg2, vti->response, vti->response_len);
    
    sgs[0] = &sg1;
    sgs[1] = &sg2;

    /* Add request_buffer to the virtqueue */

    if (virtqueue_add_sgs(dev->vq, sgs, 1, 1, vti, GFP_ATOMIC)) {
        kfree(request_buffer);
        return ENODEV;
    }

    /* Kick the virtqueue to send the data */
    virtqueue_kick(dev->vq);

    return 0;
}



static int virtio_tpm_send(struct tpm_chip *chip, u8  *buf, size_t buflen)
{
    /* 使用Virtio TPM设备的virtqueue发送数据 */
    /* ... */
    int rc;
    struct virtio_tpm_info *vti = dev_get_drvdata(&chip->dev);


    printk(KERN_INFO "\nMessage content: ");
    for (size_t i = 0; i < buflen; ++i) {
       printk(KERN_INFO "%02x", buf[i]);
    }
    printk(KERN_INFO "\n");

    rc= send_command(vti,buf,buflen);
    return rc;
}

static int virtio_tpm_recv(struct tpm_chip *chip, u8 *buf, size_t buflen)
{
    /* 使用Virtio TPM设备的virtqueue接收数据 */
    /* ... */
    return 0;
}





//----------------------------------------------------------------

static u8 virtio_tpm_status(struct tpm_chip *chip)
{
    /* 获取TPM的状态 */
    /* 从Virtio TPM设备读取状态信息 */
    /* 例如，可以通过读取特定的寄存器或使用Virtio的控制接口来获取状态 */
    /* ... */

    return 0;
}

static void virtio_tpm_ready(struct tpm_chip *chip)
{
    /* 准备TPM以接收新的命令或操作 */
    /* 这可能涉及到发送一个特定的命令给Virtio TPM设备，使其进入准备状态 */
    /* ... */


}

static void virtio_tpm_update_timeouts(struct tpm_chip *chip,
                                       unsigned long *timeout_cap)
{
    /* 更新TPM操作的超时设置 */
    /* 根据timeout_cap更新Virtio TPM设备的超时配置 */

}

static bool virtio_tpm_req_canceled(struct tpm_chip *chip, u8 status)
{
    /* 检查TPM请求是否被取消 */
    /* 根据status和其他上下文信息判断请求是否被取消 */
    /* ... */



    return false;
}

static int request_locality(struct tpm_chip *chip, int loc)
{
    /* 请求TPM的本地性 */
    /* 根据loc参数请求Virtio TPM设备的本地性 */
    /* ... */



    return 0;
}

static int release_locality(struct tpm_chip *chip, int loc)
{
    /* 释放TPM的本地性 */
    /* 根据loc参数释放Virtio TPM设备的本地性 */
    /* ... */


    return 0;
}

static void virtio_tpm_clkrun_enable(struct tpm_chip *chip, bool value)
{
    /* 启用或禁用TPM的时钟运行 */
    /* 根据value参数启用或禁用Virtio TPM设备的时钟运行 */
    /* ... */

    /* 假设我们设置了一个时钟运行控制位 */
   
}




//-----------------------------------------------------------------


static void virtio_tpm_recv_cb(struct virtqueue *vq)
{
    struct virtio_tpm_info *vi = vq->vdev->priv;
   // uint8_t *buf;
    unsigned int len;
    uint8_t numss[] = {0x80, 0x81, 0x82, 0x83, 0x84};

    while ((virtqueue_get_buf(vq, &len)) != NULL) {
        /* process the received data */
        // Process TPM commands and responses
        // You need to implement the actual TPM command processing here
        for (size_t i = 0; i < len; i++) {
            printk(KERN_INFO "my_module: buf[%zu] = 0x%X\n", i,vi->response[i]);
        }
        printk(KERN_INFO "---------------------------------------------------");
        for (size_t i = 0; i < sizeof(numss); i++) {
            printk(KERN_INFO "my_module: numss[%zu] = 0x%X\n", i, numss[i]);
        }
    }
}



static const struct tpm_class_ops virtio_tpm = {
        .flags = TPM_OPS_AUTO_STARTUP,    //
        .status = virtio_tpm_status,      //
        .recv = virtio_tpm_recv,          //
        .send = virtio_tpm_send,          //
        .cancel = virtio_tpm_ready,          //  
        .update_timeouts = virtio_tpm_update_timeouts,    
        .req_complete_mask = TPM_STS_DATA_AVAIL | TPM_STS_VALID,  //
        .req_complete_val = TPM_STS_DATA_AVAIL | TPM_STS_VALID, //
        .req_canceled = virtio_tpm_req_canceled,     //
        .request_locality = request_locality,           //
        .relinquish_locality = release_locality,
        .clk_enable = virtio_tpm_clkrun_enable,    //
}; 








static int virtio_tpm_core_init(struct virtio_device *vdev, struct virtio_tpm_info vti){
    struct device dev = vdev->dev;
    struct tpm_chip *chip;
    int rc;


    chip =   tpmm_chip_alloc(&dev,&virtio_tpm);
    dev_set_drvdata(&chip->dev,&vti);



    //TODO : acpi support

    rc = tpm2_probe(chip);
 
 
                        
    return 0;
}



static int virtio_tpm_probe(struct virtio_device *vdev)
{
    struct virtio_tpm_dev *dev = NULL;
    struct virtio_tpm_info * vi; 
    //struct _tpm_header_test *hello_cmd;
    
    

    /* initialize device data */
    dev = kzalloc(sizeof(struct virtio_tpm_dev), GFP_ATOMIC);
    if (!dev)
        return -ENOMEM;
    vi = kzalloc(sizeof(struct virtio_tpm_info), GFP_ATOMIC);
    if (!dev)
        return -ENOMEM;
    
    
    dev->priv = vi;
    vi->vtdev = dev;
    vi->response_len = TPM_RESPONSE_LEN;    
    /* the device has a single virtqueue */
    dev->vq = virtio_find_single_vq(vdev, virtio_tpm_recv_cb, "input");
    if (IS_ERR(dev->vq)) {
        kfree(dev);
        return PTR_ERR(dev->vq);
    }
    vdev->priv = dev;

    /* from this point on, the device can notify and get callbacks */
    virtio_device_ready(vdev);

    virtio_tpm_core_init(vdev,*vi);    

    /* Send a "hello" message to the backend device */
    //send_command(vdev, "hello");
    return 0;
}

static void virtio_tpm_remove(struct virtio_device *vdev)
{
    struct virtio_tpm_dev *dev = vdev->priv;
    char *buf;

    /*
     * disable vq interrupts: equivalent to
     * vdev->config->reset(vdev)
     */
    virtio_reset_device(vdev);

    /* detach unused request_buffer */
    while ((buf = virtqueue_detach_unused_buf(dev->vq)) != NULL) {
        kfree(buf);
    }

    /* remove virtqueues */
    vdev->config->del_vqs(vdev);

    kfree(dev);
}

static const struct virtio_device_id id_table[] = {
    { .device = VIRTIO_ID_TPM, .vendor = VIRTIO_DEV_ANY_ID },
    { 0 },
};

static struct virtio_driver virtio_tpm_driver = {
    .driver.name =  KBUILD_MODNAME,
    .id_table =     id_table,
    .probe =        virtio_tpm_probe,
    .remove =       virtio_tpm_remove,
};

module_virtio_driver(virtio_tpm_driver);
MODULE_DEVICE_TABLE(virtio, id_table);
MODULE_DESCRIPTION("Virtio TPM driver");
MODULE_LICENSE("GPL");