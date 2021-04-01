#!/user/bin/env python
# -*- coding:utf-8 -*-

import sys
import EarliestDeadlineFirst
import LeastLaxityFirst


class GetCloseTime:
    """ design the close time by itself """
    def __init__(self, dictionary):
        self.dictionary = dictionary

    def greatest_common_divisor(self, _left, _right):
        return _left if _right == 0 else self.greatest_common_divisor(_right, _left % _right)

    def lowest_common_multiple(self):
        temp_result = 1
        for value in self.dictionary.values():
            temp_result = value[1] * temp_result / self.greatest_common_divisor(value[1], temp_result)
        return temp_result


class TimeError(Exception):
    """
    Self-defined Exception :
        Judging whether the processing time and cycle of real-time tasks satisfy the conditions
    """
    def __init__(self, message):
        self.message = message

def is_execute(dictionary):
    sum = 0
    for value in dictionary.values():
        sum += value[0] / value[1]
    return sum

if __name__ == "__main__":
    task_dictionary = {"A": [10, 30],
                       "B": [20, 60],
                       "C": [30, 90]}
    if is_execute(task_dictionary) > 1:
        raise TimeError("error, scheduling finish!")
    user_choose = input("Please enter your choice"
                        "(1 : EarlistDeadlineFirist) or (2 : LeastLaxityFirst) : ")
    close_time_object = GetCloseTime(task_dictionary)
    close_time = close_time_object.lowest_common_multiple()
    if int(user_choose) == 1:
        current_time = 0
        name_list = []
        period_time = []
        central_processing_unit_time = []
        remain_time = []
        current_period = []
        tcb = EarliestDeadlineFirst.TaskControlBlock(task_dictionary,
                               name_list,
                               period_time,
                               central_processing_unit_time,
                               remain_time,
                               current_period)


        while tcb.CURRENT_TIME < close_time:
            tcb.scheduling(name_list,
                           period_time,
                           central_processing_unit_time,
                           remain_time,
                           current_period)
    else:
        current_time = 0
        name_list = []
        period_time = []
        central_processing_unit_time = []
        remain_time = []
        current_period = []
        laxity_time = []
        tcb = LeastLaxityFirst.TaskControlBlock(task_dictionary,
                               name_list,
                               period_time,
                               central_processing_unit_time,
                               remain_time,
                               current_period,
                               laxity_time)
        while tcb.CURRENT_TIME < close_time:
            tcb.scheduling(name_list,
                           period_time,
                           central_processing_unit_time,
                           remain_time,
                           current_period,
                           laxity_time)