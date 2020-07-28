import requests
import json
import matplotlib.pyplot as plt
import numpy as np
from selenium import webdriver
from time import sleep
from selenium.webdriver.firefox.options import Options
#from selenuim.webdriver.chrome.options import Options


def get_data_group(group_name):
    headers = {
        'X-M2M-Origin': 'admin:admin',
        'Content-type': 'application/json'
    }
    group_uri = server+group_name
    response = requests.get(group_uri, headers=headers)
    try:
        _resp = json.loads(response.text)
    except:
        print("lol:error")
    val = []
    val.append(_resp['m2m:cin']['lt'])
    val.append(_resp['m2m:cin']['con'])
    val.append(group_name)
    data.append(val)
    return response.status_code, _resp


if __name__ == "__main__":
    # plt.ion()
    data = []
    ymax = []
    ymin = []
    opts = Options()
    opts.set_headless()
    assert opts.headless  # Operating in headless mode
    print("running the program")
    driver = webdriver.Firefox()
    driver.get(
        "http://onem2m.iiit.ac.in:443/webpage/welcome/index.html?context=/~&cseId=in-cse")
    # sleep(3)
    driver.find_element_by_tag_name("button").click()
    # sleep(5)
    driver.find_element_by_xpath("//li/ul/li[27]").click()
    # sleep(4)
    driver.find_element_by_xpath("//li/ul/li[27]/ul/li[3]").click()
    # sleep(6)
    ele1 = driver.find_elements_by_xpath("//li/ul/li[27]/ul/li[3]/ul/*")
    print(ele1)
    print(len(ele1))
    lst1 = []
    for i in ele1:
        lst1.append(i.text)
    # print(lst1)
    server = "http://onem2m.iiit.ac.in:443/~/in-cse/in-name/Team25_Lighting_dim_control_based_on_speaker_presentation/node_1/"
    for i in lst1:
        get_data_group(i)
    values = []
    with open('temp.txt', 'w') as f:
        f.truncate()
        # for k in range(100):
        # j= data[len(data)-100+k]
        for j in data:
            if(j[1] != "test_instance"):
                arr = j[0].split("T")
                date = arr[0][6:8]+"-"+arr[0][4:6]+"-"+arr[0][0:4]
                # time_val =  str(530+int(arr[1][0:4]))
                time_val = arr[1][0:4]
                time = time_val[0:2] + ":" + time_val[2:4]
                f.write("{} {} {} {}\n".format(date,time, j[1], j[2]))
                # t = float(j[1])
                if(j[1] == "Projector ON"):
                    t = 1
                elif (j[1] == "Projector OFF"):
                    t = 0
                values.append(t)
    fig = plt.figure()
    plt.plot(values)
    plt.show()

    plt.close()
    print("ssss")
    # fig.savefig(out)
