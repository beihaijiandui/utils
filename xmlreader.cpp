//用于解析非标准xml的工具函数

//将xml文件读取到string中
int read_khxx_file(const std::string& khxx_file, std::string& buf) {
    std::ifstream file;
    std::string bufFromFile;

    file.open(khxx_file);
    if (!file.is_open()) {
        std::cout << "error in open file khxx_file";
        exit(1);
    }

    while (!file.eof()) {
        file >> bufFromFile;
        buf += bufFromFile;
    }

    std::cout << buf << std::endl;

    file.close();

    return 0;
}

//解析输入的string，每次输入要解析的目标字符串
int parse_khxx_file(const std::string& buf, const char* name, char* value){
   
    std::string  beginName, endName;

    int begin, end;

    if (name == NULL || value == NULL) {
        std::cout << "name and value is invalid" << std::endl;
        exit(1);
    }

    beginName.append("<");
    beginName.append(name);
    beginName.append(">");

    endName.append("</");
    endName.append(name);
    endName.append(">");


    begin = buf.find(beginName);
    end = buf.find(endName);

    //std::cout << begin << " " << end << std::endl;

    if (begin < 0 || end < 0) {
        std::cout << "can't find " << name << " Item" << std::endl;
        exit(1);
    }
    if (begin >= end) {
        std::cout << "error in find " << name << " Item" << std::endl;
        exit(1);
    }
    for (int i = 0; i < end - begin - beginName.length(); i++) {
        value[i] = buf.at(begin + beginName.length() + i);
    }

    value[end - begin - beginName.length()] = '\0';

    return 0;
}

//读取证券代码
int read_zqdm_file(const std::string& zqdm_path, std::vector<ZqdmInfo>& vecZQDM) {
    std::ifstream is(zqdm_path, std::ifstream::in);
    if (!is.is_open())
    {
        std::cout << "open file failed! filename: " << zqdm_path << std::endl;
        return -1;
    }

    char content[256];
    while (is.getline(content, sizeof(content)))            //每次读取一行
    {
        ZqdmInfo ZqdmInfo_;
        //memset(&ZQDMUnit_, 0x00, sizeof(ZQDMUnit_));

        char* token = nullptr;

        if ((token = std::strtok(content, ",")) != nullptr) {         //对于每一行用,分隔符去parse，可按需换成tab键
            ZqdmInfo_.strType = token;
        }
        if ((token = std::strtok(NULL, ",")) != nullptr) {
            std::string market = token;
            if (market == "SH") {
                ZqdmInfo_.nJys = ATPMarketIDConst::kShangHai;
            }
            else if (market == "SZ") {
                ZqdmInfo_.nJys = ATPMarketIDConst::kShenZhen;
            }
            else {
                std::cout << "market invalid:"<< market << std::endl;
                return -1;
            }
        }
        if ((token = std::strtok(NULL, ",")) != nullptr) {
            ZqdmInfo_.strZqdm = token;
        }
        if ((token = std::strtok(NULL, ",")) != nullptr) {
            std::string mmfx = token;
            switch (mmfx[0]) {
            case '1':
                ZqdmInfo_.cMmfx = ATPSideConst::kBuy;             //竞价买入
                break;
            case '2':
                ZqdmInfo_.cMmfx = ATPSideConst::kSell;            //竞价卖出 
                break;
            case 'N':
                ZqdmInfo_.cMmfx = ATPSideConst::kAntiRepo;        //逆回购
                break;
            case 'F':
                ZqdmInfo_.cMmfx = ATPSideConst::kDefault;         //发行
                break;
            case 'P':
                ZqdmInfo_.cMmfx = '1';                            //配售深圳
                break;
            case 'Q':
                ZqdmInfo_.cMmfx = '2';                            //配售上海
                break;
            default:
                std::cout << "mmfx invalid:" << mmfx << std ::endl;
                return -1;
            }
        }
        if ((token = std::strtok(NULL, ",")) != nullptr) {
            ZqdmInfo_.lfWtsl = std::stold(token);
        }
        if ((token = std::strtok(NULL, ",")) != nullptr) {
            ZqdmInfo_.lfWtjg = std::stold(token);
        }
        if ((token = std::strtok(NULL, ",")) != nullptr) {
            ZqdmInfo_.ord_type = token[0];
        }
        vecZQDM.push_back(ZqdmInfo_);
    }
    is.close();
  return 0;
}


int main(int argc, char* argv[])
{
    //读取客户信息配置文件
    int res;
    std::string khxx_buf;
    res = read_khxx_file("../data/khxx_file.xml", khxx_buf);
    if (res != 0) return -1;

    res = parse_khxx_file(khxx_buf, "AGWLocationM", locationM);
    res = parse_khxx_file(khxx_buf, "AGWUser", g_szUser);
    res = parse_khxx_file(khxx_buf, "AGWPassword", g_szPassword);
    res = parse_khxx_file(khxx_buf, "Khh", g_szKhh);
    res = parse_khxx_file(khxx_buf, "ZJZH", g_szZjzh);
    res = parse_khxx_file(khxx_buf, "Yybbm", g_szYybdm);
    res = parse_khxx_file(khxx_buf, "SHGdh", g_szShGdh);
    res = parse_khxx_file(khxx_buf, "SZGdh", g_szSzGdh);
    res = parse_khxx_file(khxx_buf, "TDPassword", g_szPwd);
    res = parse_khxx_file(khxx_buf, "wtfs", tmpStrWtfs);
    if (res != 0) return -1;
    g_vecLocations.clear();
    g_vecLocations.push_back(std::string(locationM));
    g_cWtfs = tmpStrWtfs[0];

    //debug
    std::cout << g_vecLocations.size() << g_vecLocations[0] << std::endl;
    std::cout << g_szUser << std::endl;
    std::cout << g_szPassword << std::endl;
    std::cout << g_szKhh << std::endl;
    std::cout << g_szZjzh << std::endl;
    std::cout << g_szYybdm << std::endl;
    std::cout << g_szShGdh << std::endl;
    std::cout << g_szSzGdh << std::endl;
    std::cout << g_szPwd << std::endl;
    std::cout << g_cWtfs << std::endl;
    //读取证券代码数据
    res = read_zqdm_file("../data/zqdm.txt", g_vecZqdmInfo);
  return 0;
 }
