#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio/consumer.h>
#include <asm/param.h>
#include <linux/kthread.h>
#include <net/sock.h>

#define GPIO_NUM 23

static dev_t led_one_ad_dev_number;
static struct cdev *driver_object;
static struct class *blinkdrvat_class;
static struct device *led_one_ad_dev;

static struct gpio_desc *desc;

static atomic_t freqSadkowski = ATOMIC_INIT(0);

static struct task_struct *thread_id;
static wait_queue_head_t wq;

struct mutex write_init_mutex;
struct completion on_exit;

static int blinkfunction(void* data){
    allow_signal( SIGTERM );
    bool led = false;    
    int prev_freq = -1;
    unsigned long jiffies;
    unsigned long timeout;
    while (!kthread_should_stop()) {
        int freq = atomic_read(&freqSadkowski);
        if ( freq != prev_freq) {
            prev_freq = freq;
        }
         
        if (freq == 0) {
            gpiod_set_value(desc, 0);
            timeout = HZ/100;        
            timeout = wait_event_interruptible_timeout(wq, ((freq != prev_freq) || (timeout == 0)), timeout);
            if (timeout == -ERESTARTSYS) {
                break;
            }
            }
        else if (freq == 51) {
            gpiod_set_value(desc, 1);
            timeout = HZ/100;        
            timeout = wait_event_interruptible_timeout(wq, ((freq != prev_freq) || (timeout == 0)), timeout);
            if (timeout == -ERESTARTSYS) {
                break;
            }
        } 
        else {  
            led = !led;
            gpiod_set_value(desc, led);
            timeout = HZ/(2*freq);
            timeout = wait_event_interruptible_timeout(wq, ((freq != prev_freq) || (timeout == 0)), timeout);
            if (timeout == -ERESTARTSYS) {
                break;
            }
        }       
    }
    kthread_complete_and_exit( &on_exit, 0 );
    return 0;
}

static int driver_open( struct inode *geraete_datei, struct file *instanz )
{
    return 0;
}

static int driver_close( struct inode *geraete_datei, struct file *instanz )
{    
    return 0;
}

static ssize_t driver_read(struct file *instanz, char __user *user,
        size_t count, loff_t *offset)
{
    unsigned long not_copied, to_copy;
    int pfreq;
    char cfreq[3];
    pfreq = atomic_read(&freqSadkowski);
    sprintf(cfreq, "%d", pfreq);
    to_copy = min(count, sizeof(cfreq));
    not_copied = copy_to_user(user, cfreq, to_copy);		
    *offset += (to_copy-not_copied);
    return to_copy - not_copied;
}

static ssize_t driver_write(struct file *instanz, const char __user *kvalue,
    size_t count, loff_t *offset)
{
    unsigned long not_copied, to_copy;
    char cfreq[3];
    int new_freq = 0;
    to_copy = min(count, sizeof(cfreq));
    not_copied = copy_from_user(&cfreq, kvalue, to_copy);
    if (!sscanf(cfreq, "%d", &new_freq)) return -EINVAL;
    
    if (new_freq < 0){
        new_freq = 0;
    }
    else if (new_freq >= 51){
        new_freq = 51;
    }

    atomic_set(&freqSadkowski, new_freq);

    mutex_lock(&write_init_mutex);

    wake_up_interruptible(&wq);
          
    mutex_unlock(&write_init_mutex);
    
    *offset += to_copy - not_copied;
    return to_copy - not_copied;
}


static struct file_operations fops = {
    .owner= THIS_MODULE,
    .read= driver_read,
    .write= driver_write,
    .open= driver_open, 
    .release= driver_close,
};

static int __init mod_init(void)
{
    desc = gpio_to_desc(GPIO_NUM);
    if (!desc){
        printk("GPIO %d not available\n", GPIO_NUM);
        return -EIO;
    }
    if (gpiod_direction_output(desc, 0) < 0) {
        dev_err(led_one_ad_dev, "Failed to set GPIO direction\n");
        return -EINVAL;
    }    

    init_waitqueue_head(&wq);
    init_completion(&on_exit);
    mutex_init(&write_init_mutex);
    atomic_set(&freqSadkowski, 0);
    thread_id = kthread_create(blinkfunction, NULL, "blinkfunction");
    if (thread_id == 0)
        return -EIO;
    wake_up_process(thread_id);
    

    if (alloc_chrdev_region(&led_one_ad_dev_number,0,1,"blinkdrvat.c")<0)
        return -EIO;
    driver_object = cdev_alloc();
    if (driver_object==NULL)
        goto free_device_number;
    driver_object->owner = THIS_MODULE;
    driver_object->ops = &fops;
    if (cdev_add(driver_object,led_one_ad_dev_number,1))
        goto free_cdev;
    //  Sysfs entry
    blinkdrvat_class = class_create(THIS_MODULE, "blinkdrvat.c");
    if (IS_ERR(blinkdrvat_class)) {
        pr_err("blinkdrvat: no udev support\n");
        goto free_cdev;
    }
    led_one_ad_dev = device_create(blinkdrvat_class, NULL, led_one_ad_dev_number,
            NULL, "%s", "led_one_ad");
    if (IS_ERR(led_one_ad_dev)) {
        pr_err("led_one_ad: device_create failed\n");
        goto free_class;
    }
    return 0;

free_class:
    class_destroy(blinkdrvat_class);
free_cdev:
    kobject_put(&driver_object->kobj);
free_device_number:
    unregister_chrdev_region(led_one_ad_dev_number, 1);
    return -EIO;
}

static void __exit mod_exit(void)
{
    if (thread_id != NULL) {
        kill_pid(task_pid(thread_id), SIGTERM, 1);
        thread_id = NULL;
        wait_for_completion( &on_exit );
    }
    dev_info( led_one_ad_dev, "driver_close called\n" );
    gpiod_set_value(desc, 0);
    gpiod_put(desc);    
    device_destroy(blinkdrvat_class, led_one_ad_dev_number);
    class_destroy(blinkdrvat_class);
    cdev_del(driver_object);
    unregister_chrdev_region(led_one_ad_dev_number, 1);
    return;
}

module_init(mod_init);
module_exit(mod_exit);

/* Metainformation */
MODULE_AUTHOR("Dennis  Sadkowski");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A driver to make a LED blink on the GPIO23 of a Raspberry Pi ");