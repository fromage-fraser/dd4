
int main(int argc, char **argv)
{

int val, val2;
int low_bit, high_bit;
char buf[20];

val = 4096;
val2 = 0;

low_bit = (val << 24);
high_bit = (val >> 8);
val2 = (val2 | low_bit);
val2 = (val2 | high_bit);

sprintf(buf, "val2 = %d\n", val2);
printf(buf);

}
