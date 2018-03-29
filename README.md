# NET Indicator

[![Build Status](https://travis-ci.org/dawidd6/indicator-net.svg?branch=master)](https://travis-ci.org/dawidd6/indicator-net)

![Screen](screen.png)

### Installing from PPA

```sh
sudo add-apt-repository ppa:dawidd0811/indicators
sudo apt-get update
sudo apt-get install indicator-net
```

### Building from source

```sh
sudo apt install libappindicator3-dev
make
sudo make install
```

### How to move indicator to the left?

And why one would want to do that?

Well, this indicator is built in the way, that it's constantly changing its width.

This situation can be frustrating when one have e.g. Steam indicator next to the left.

There is a solution, rather simple. Move indicator-net to the left.

```sh
echo "indicator-net=-1" |sudo tee -a "/usr/share/indicator-application/ordering-override.keyfile"
```

Now logout and log back in. Should be fine from then.
