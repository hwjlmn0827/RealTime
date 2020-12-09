import re

class Trans:
    def __init__(self,file1,file2):
        self.input = file1
        self.char_schedual = file2
        self.tasks = []
        self.tickResults = {}
        self.bound = 0
        self.allSteps = ''

    def findAll(self, all_str, sub_str):
        index_list = []
        index = all_str.find(sub_str)
        sub_len = len(sub_str)
        while index != -1:
            index_list.append(index)
            index = all_str.find(sub_str, index + sub_len)
        if len(index_list) > 0:
            return index_list
        else:
            return -1

    def readFiles(self):
        f1 = open(self.input,"r",encoding="utf-8")
        i = 0
        for each in f1.readlines():
            i += 1
            if i == 2:
                self.bound = int(each.strip('\n'))
            if str(each).startswith("\n") is False and i > 3:
                self.tasks.append(each.replace(' ','').split(",")[0])
        f2 = open(self.char_schedual,"r",encoding="utf-8")
        self.allSteps = f2.read()

    def HtmlHeader(self):
        html = "<html><body><style type=\"text/css\">\n"
        for each in open("output.css", "r").readlines():
            html += each
        html += "</style>"
        f = open("output.html", "w", encoding="utf-8")
        f.write(html)

    def outPutTickByHTML(self):
        self.HtmlHeader()
        html = "<div id='dpic'><ul><li class='name'></li>"
        for each in range(1, self.bound + 1):
            html += "<li>%s</li>" % (each)
        html += "</ul>"
        for task in self.tasks:
            self.tickResults[task] = self.findAll(trans.allSteps, task) 
        for each in self.tickResults.keys():
            if each != "msec":
                html += "<ul><li class='name'>%s</li>" % (each)
                cnt = 0
                res = ""
                for i in range(1, self.bound + 1):
                    if i in self.tickResults[each]:
                        if i - 1 in self.tickResults[each] or i - 1 == 0:
                            html += "<li class='up'></li>"
                        else:
                            html += "<li class='upl'></li>"
                    else:
                        if i - 1 not in self.tickResults[each] or i - 1 == 0:
                            html += "<li class='down'></li>"
                        else:
                            html += "<li class='downl'></li>"
                    if i in self.tickResults[each]:
                        cnt += 1
                    res += "<li>%s</li>" % (cnt)
                html += "</ul>"
        html += "<hr>"
        html += "</div>"
        html += "</body></html>"
        with open("output.html", "a+", encoding='utf-8') as f:
            f.write(html)
            f.flush()
            f.close()
        html += "<ul><li class='name'>msec</li><li class='upl'></li>" 
        for i in range(self.bound):
            html += "<li class='up'></li>"

if __name__ == "__main__":
    trans = Trans("input.txt", "char_schedual.txt")
    trans.readFiles()
    trans.outPutTickByHTML()
