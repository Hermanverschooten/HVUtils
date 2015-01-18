# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'hvutils/version'

Gem::Specification.new do |spec|
  spec.name          = "hvutils"
  spec.version       = Hvutils::VERSION
  spec.authors       = ["Herman verschooten"]
  spec.email         = ["Herman@verschooten.net"]
  spec.summary       = %q{A number of utilities, getMAC,...}
  spec.description   = %q{A number of utilities, including getting the MAC address for an interface.}
  spec.homepage      = "https://github.com/Hermanverschooten/HVUtils"
  spec.license       = "MIT"

  spec.files         = `git ls-files -z`.split("\x0")
  spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
  spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
  spec.require_paths = ["lib", "ext"]
  spec.extensions    = Dir['ext/**/extconf.rb']

  spec.add_development_dependency "bundler", "~> 1.7"
  spec.add_development_dependency "rake", "~> 10.0"
  spec.add_development_dependency "rake-compiler"
  spec.add_development_dependency "rspec"
end
