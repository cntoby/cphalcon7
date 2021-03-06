Phalcon7 Framework
=================

Phalcon7 is a web framework implemented as a C extension offering high performance and lower resource consumption.

Phalcon7 是什么？
-----------------

Phalcon7 是继承自 Phalcon 1.3.x，开源、全功能栈、使用 C 编写、针对 PHP 7 优化的高性能框架。
开发者不需要学习和使用 C 语言的功能， 因为所有的功能都以 PHP 类的方式暴露出来，可以直接使用。
Phalcon7 源自 Phalcon 所以具备了 Phalcon 所有与生俱来的特性，是松耦合的，可以根据项目的需要任意使用其他对象。

Phalcon7 不只是为了卓越的性能, 我们的目标是让它更加健壮，拥有更加丰富的功能以及更加简单易于使用！

Phalcon7 版权申明
------------------
Phalcon7 作为 Phalcon 1.3 系列的继承者，所以版权理所当然的属于 Phalcon 官方团队所有。

Get Started
-----------

Phalcon is written in C with platform independence in mind. As a result, Phalcon is available on Microsoft Windows, GNU/Linux, and Mac OS X. You can either download a binary package for the system of your choice or build it from sources.

### Linux/Unix/Mac

On a Unix-based platform you can easily compile and install the extension from sources.

#### Requirements
Prerequisite packages are:

* PHP 7.0.x development resources
* GCC compiler (Linux/Solaris) or Xcode (Mac)

Ubuntu:

```bash
# if enable qrcode
sudo apt-get install libqrencode-dev libzbar-dev imagemagick libmagick++-dev libmagickwand-dev libmagickcore-dev libpng12-dev

sudo add-apt-repository ppa:ondrej/php
sudo apt-get install php7.0-dev libpcre3-dev gcc make

# or compilation
----------------
cd php-src
 ./buildconf --force
./configure --prefix=/usr/local/php --with-config-file-path=/usr/local/php/etc --with-fpm-user=www-data --with-fpm-group=www-data --with-pdo-pgsql --with-pdo-mysql --with-pdo-sqlite  --with-iconv-dir --with-freetype-dir --with-jpeg-dir --with-png-dir --with-zlib --with-libxml-dir=/usr --enable-xml --disable-rpath --enable-bcmath --enable-shmop --enable-sysvsem --enable-inline-optimization --with-curl --enable-mbregex --enable-mbstring --with-mcrypt --enable-ftp --with-gd --enable-gd-native-ttf --with-openssl --with-mhash --enable-pcntl --enable-sockets --with-xmlrpc --enable-zip --enable-soap --without-pear --with-gettext --disable-fileinfo --enable-maintainer-zts --enable-phpdbg-debug --enable-debug
make -j4
sudo make install
```

Compilation
-----------

Follow these instructions to generate a binary extension for your platform:

```bash
git clone git://github.com/dreamsxin/cphalcon7.git
cd cphalcon7/ext
phpize
make -j4
sudo make install
# or
/usr/local/php/bin/phpize
./configure CFLAGS="-g3 -O0 -std=gnu90 -Wall -Werror -Wno-error=uninitialized" --with-php-config=/usr/local/php/bin/php-config
# or
./configure CFLAGS="-g3 -O0 -std=gnu90 -Wall -Werror -Wno-error=uninitialized" --with-php-config=/usr/local/php/bin/php-config --without-qrcode
```

Add the extension to your php.ini:

```ini
extension=phalcon.so
```

Test:

```shell
php --ri phalcon7
```

Finally, restart the webserver.

Current Build Status
--------------------

Phalcon Framework is built under the Travis CI service. Every commit pushed to this repository will queue a build into the continuous integration service and will run all PHPUnit tests to ensure that everything is going well and the project is stable. The current build status is:

[![Build Status](https://secure.travis-ci.org/dreamsxin/cphalcon7.png?branch=master)](http://travis-ci.org/dreamsxin/cphalcon7)

Meet the Incubator
-----------
Our community is developing amazing extra features for Phalcon every day via [Incubator](https://github.com/phalcon/incubator). There are resources to enhance your experience with the framework and that enlarge the main features.

Just give it a try and help us improve Phalcon even more!

External Links
--------------

* [中文帮助](https://github.com/dreamsxin/cphalcon7/wiki)
* [捐贈名單（Donation）](https://github.com/dreamsxin/cphalcon7/blob/master/DONATE.md)
* PHP5 系列使用 [Phalcon 1.3](https://github.com/dreamsxin/cphalcon)

License
-------
Phalcon is open source software licensed under the New BSD License. See the docs/LICENSE.txt file for more information.
