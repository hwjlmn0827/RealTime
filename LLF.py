#!/user/bin/env python
# -*- coding:utf-8 -*-

import sys


class TaskControlBlock:
    CURRENT_TIME = 0

    def __init__(self, dictionary,
                 name_list,
                 period_time,
                 central_processing_unit_time,
                 remain_time,
                 current_period,
                 laxity_time):
        for key in dictionary.keys():
            name_list.append(key)
            period_time.append(dictionary.get(key)[1])
            central_processing_unit_time.append(dictionary.get(key)[0])
            remain_time.append(dictionary.get(key)[0])
            current_period.append(1)
            laxity_time.append(dictionary.get(key)[1] - dictionary.get(key)[0] - self.CURRENT_TIME)

    @staticmethod
    def get_index_of_min(least_laxity_task_list, minimum):
        return least_laxity_task_list.index(minimum)

    @staticmethod
    def get_another_index_of_min(least_laxity_task_list, minimum):
        least_laxity_task_list[least_laxity_task_list.index(minimum)] = sys.maxsize
        return least_laxity_task_list.index(min(least_laxity_task_list))

    @staticmethod
    def is_exit(temp_list):
        for element in temp_list:
            if element != sys.maxsize:
                return False
        return True

    def scheduling(self, name_list,
                   period_time,
                   central_processing_unit_time,
                   remain_time,
                   current_period,
                   laxity_time):
        least_laxity_task = laxity_time.index(min(laxity_time))
        temp_list = []
        for i in laxity_time:
            temp_list.append(i)
        if self.CURRENT_TIME < period_time[least_laxity_task] * (current_period[least_laxity_task] - 1):
            while self.CURRENT_TIME < period_time[least_laxity_task] * \
                    (current_period[least_laxity_task] - 1):
                least_laxity_task = self.get_another_index_of_min(temp_list, min(temp_list))
                if self.is_exit(temp_list=temp_list):
                    exit(0)
            another_task = self.get_another_index_of_min(temp_list, min(temp_list))
            if remain_time[least_laxity_task] \
                    <= laxity_time[another_task]:
                running_time = remain_time[least_laxity_task]
            else:
                running_time = laxity_time[another_task]
            remain_time[least_laxity_task] -= running_time
            print(name_list[least_laxity_task], self.CURRENT_TIME, running_time)
            self.CURRENT_TIME += running_time
            if remain_time[least_laxity_task] == 0:
                current_period[least_laxity_task] += 1
                remain_time[least_laxity_task] = central_processing_unit_time[least_laxity_task]
            i = 0
            while i < laxity_time.__len__():
                laxity_time[i] = current_period[i] * period_time[i] - \
                                 remain_time[i] - self.CURRENT_TIME
                i += 1
            print(laxity_time)
        else:
            another_task = self.get_another_index_of_min(temp_list, min(temp_list))
            if remain_time[least_laxity_task] \
                    <= temp_list[another_task]:
                running_time = remain_time[least_laxity_task]
            else:
                running_time = laxity_time[another_task]
            remain_time[least_laxity_task] -= running_time
            print(name_list[least_laxity_task], self.CURRENT_TIME, running_time)
            self.CURRENT_TIME += running_time
            if remain_time[least_laxity_task] == 0:
                current_period[least_laxity_task] += 1
                remain_time[least_laxity_task] = central_processing_unit_time[least_laxity_task]
            i = 0
            while i < laxity_time.__len__():
                laxity_time[i] = current_period[i] * period_time[i] - \
                                 remain_time[i] - self.CURRENT_TIME
                i += 1
            print(laxity_time)


if __name__ == "__main__":
    task_dictionary = {"A": [10, 30],
                       "B": [25, 75],
                       "C": [30, 90],}
    current_time = 0
    name_list = []
    period_time = []
    central_processing_unit_time = []
    remain_time = []
    current_period = []
    laxity_time = []
    tcb = TaskControlBlock(task_dictionary,
                           name_list,
                           period_time,
                           central_processing_unit_time,
                           remain_time,
                           current_period,
                           laxity_time)