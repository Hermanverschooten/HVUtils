require 'get_mac'

describe GetMac do
  it 'returns a mac address as string' do
    expect(GetMac.string('en0')).to eql('685B35B24DD5')
  end
end
