local saci = require("saci")

require("saci.types")
require("saci.platform")

local modules = { require("saci.renderer.base") }

local instances = {}

for _, mod in ipairs(modules) do
  table.insert(instances, mod(saci));
end

for _, m in ipairs(instances) do
  m.init()
end
