#!/usr/bin/env python3

import requests

def main(sensor_ip, sensor_location, influxdb_host, influxdb_name):
    result = requests.get('http://' + sensor_ip).json()
    print(result)
    requests.post('http://{}/write?db={}'.format(influxdb_host, influxdb_name),
                  data='watt,where={} value={}'.format(sensor_location, result['watt']))
    requests.post('http://{}/write?db={}'.format(influxdb_host, influxdb_name),
                  data='amp,where={} value={}'.format(sensor_location, result['amp']))



def parse_args():
    import argparse
    parser = argparse.ArgumentParser('Query consomation over HTTP and push to InfluxDB')
    parser.add_argument('sensor_ip')
    parser.add_argument('sensor_location')
    parser.add_argument('influxdb_host')
    parser.add_argument('influxdb_name')
    return parser.parse_args()


if __name__ == '__main__':
    main(**vars(parse_args()))
