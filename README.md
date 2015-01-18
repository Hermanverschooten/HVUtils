# Hvutils

A set of small utilities.

* get_mac : returns the MAC address of the interface.
* get_ip  : returns the IPv4 address of the interface.
* arp_get : returns the MAC address for an IP on an interface.

## Installation

Add this line to your application's Gemfile:

```ruby
gem 'hvutils'
```

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install hvutils

## Usage

    Hvutils.get_mac('eth0') => mac address as string
    Hvutils.get_ip('eth0') => ip address as string
    Hvutils.arp_get('eth0', '192.168.1.1') => mac address as string

## Contributing

1. Fork it ( https://github.com/Hermanverschooten/getmac/fork )
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create a new Pull Request
