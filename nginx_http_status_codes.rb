#!/usr/bin/env ruby
require 'rubygems'
require 'munin_plugin'
require 'open-uri'

ACTIVE_STATS_URL="http://localhost:8080/status-codes"

def get_stats
  lines = open(ACTIVE_STATS_URL).read.split("\n")
  stats = lines[1..(lines.size)]
  stats = stats.collect {|str| str.split(' ') }
  stats = stats.inject({}) {|hsh, arr| hsh[arr[0].to_i] = arr[1].to_i; hsh }
  return stats
end

munin_plugin do
  graph_title "NGINX HTTP Status Codes"
  graph_vlabel "number"
  graph_category "Nginx"
  graph_args "-l 0"
  
  get_stats.each do |code,count|
    send("code_#{code}").label "#{code}"
    send("code_#{code}").type "DERIVE"
  end

  collect do
    get_stats.each do |code,count|
      send("code_#{code}").value count.to_i
    end
  end
end
