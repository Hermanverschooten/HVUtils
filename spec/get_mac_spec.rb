require 'hvutils'

describe Hvutils do
  let(:interface) {  RbConfig::CONFIG['host_os'] =~ /darwin/i ? 'en0' : 'eth0' }

  it 'returns the mac address for an interface' do
    expect(Hvutils.get_mac(interface)).to match(/[0-9A-F]{12}/)
  end

  it 'returns the ip address for an interface' do
    expect(Hvutils.get_ip(interface)).to match(/\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}/)
  end

  it 'finds the mac address of an ip on an interface' do
    if interface == 'eth0'
      ip = '10.211.55.2'
      expect(Hvutils.arp_get(interface,ip)).to match(/([0-9A-F]{2}\:){5}[0-9A-F]{2}/i)
    end
  end
end
