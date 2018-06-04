#include "StringFilter.h"

USING_NS_CC;
using namespace cocos2d::utils;

// ----------------------------------------------------------------------------------------------------------------------------------
// SFilterTreeNode

SFilterTreeNode::SFilterTreeNode()
{
	reset();
}

SFilterTreeNode::SFilterTreeNode(std::string character)
{
	reset();
	m_sCharacter = character;
}

SFilterTreeNode::~SFilterTreeNode()
{
	m_vMap.clear();						// 只是清除元素,内存并没有变化
	//_TreeMap(m_vMap).swap(m_vMap);	// shrink to fit 收缩到合适
	_TreeMap().swap(m_vMap);			// 清除并最小化它的容量，可以理解为交换技巧的变体
}

void SFilterTreeNode::reset()
{
	m_sCharacter	= "";
	m_pParent		= nullptr;
	m_bStandAlone	= false;
	m_bBeginMark	= false;
	m_bEndMark		= false;

	m_vMap.clear();
	_TreeMap().swap(m_vMap);
}

SFilterTreeNode* SFilterTreeNode::findChild(std::string& nextCharacter)
{
	// 利用 map/unordered_map 进行查找
	_TreeMapIterator SFilterTreeMapIt = m_vMap.find(nextCharacter);
	return (SFilterTreeMapIt == m_vMap.end()) ? nullptr : &SFilterTreeMapIt->second;
}

SFilterTreeNode* SFilterTreeNode::insertChild(std::string& nextCharacter)
{
	// 添加节点，并返回节点位置
	if (!findChild(nextCharacter))
	{
		m_vMap.insert(std::pair<std::string, SFilterTreeNode>(nextCharacter, SFilterTreeNode(nextCharacter)));
		return &(m_vMap.find(nextCharacter)->second);
	}
	return nullptr;
}

// ----------------------------------------------------------------------------------------------------------------------------------
// SFilterTree

SFilterTree::SFilterTree()
	: m_bMatch(false)
{ }

SFilterTree::~SFilterTree()
{ }

void SFilterTree::resetBuffer()
{
	m_strOut.clear();
	m_strBuff.clear();
}

void SFilterTree::insert(std::string& keyword)
{
	_insert(&m_nodeRoot, keyword);
	_checkMark(&m_nodeRoot, keyword);
}

void SFilterTree::insert(const char* keyword)
{
	std::string wordstr(keyword);
	insert(wordstr);
}

bool SFilterTree::find(std::string& keyword, bool replay)
{
	int iflag = 0;
	std::string strfixed = keyword;
	std::vector<stuPosMark> vposmark;
	while (iflag < strfixed.size())
	{
		// 当前树路径
		std::string strTmp = strfixed.substr(iflag, strfixed.size());
		int iBytes = StringFilter::getFirstBytes(strTmp);
		iBytes = MIN(iBytes, strTmp.size());
		iBytes = MAX(iBytes, 1);

		std::string strOutBuff = _find(&m_nodeRoot, strTmp);
		resetBuffer();
		if (strOutBuff.size() > 0)
		{
			stuPosMark posmark;
			posmark.ibegin = iflag;
			posmark.iend = iflag + strOutBuff.size();
			vposmark.push_back(posmark);
		}
		iflag += strOutBuff.size() ? strOutBuff.size() : iBytes;
	}

	// 是否替换
	if (replay && vposmark.size() > 0)
	{
		int ioffset = 0;
		int ilastoffset = 0;
		for (int i = 0; i < vposmark.size(); ++i)
		{
			int ipos1 = vposmark[i].ibegin - ioffset;
			int ipos2 = vposmark[i].iend - ioffset;

			std::string str = keyword.substr(ipos1, ipos2 - ipos1);
			int icount = StringFilter::getChartCount(str);
			keyword.replace(ipos1, ipos2 - ipos1, icount, '*');
			ilastoffset = ipos2 - ipos1 - icount;
			ioffset += ilastoffset;
		}
	}

	// 搜索完,清空缓存
	resetBuffer();

	return vposmark.size();
}

void SFilterTree::_insert(SFilterTreeNode* parent, std::string& keyword)
{
	if (!parent || keyword.size() == 0) return;

	int iBytes = StringFilter::getFirstBytes(keyword);
	iBytes = MIN(iBytes, keyword.size());
	
	std::string firstChar = keyword.substr(0, iBytes);
	SFilterTreeNode* firstNode = parent->findChild(firstChar);
	if (!firstNode)
	{
		_insertBranch(parent, keyword);
	}

	std::string restChar = keyword.substr(iBytes, keyword.size());
	_insert(firstNode, restChar);
}

void SFilterTree::_insertBranch(SFilterTreeNode* parent, std::string& keyword)
{
	int iBytes = StringFilter::getFirstBytes(keyword);
	iBytes = MIN(iBytes, keyword.size());

	std::string firstChar = keyword.substr(0, iBytes);
	SFilterTreeNode* firstNode = parent->insertChild(firstChar);
	if (firstNode)
	{
		std::string restChar = keyword.substr(iBytes, keyword.size());
		if (!restChar.empty())
		{
			_insertBranch(firstNode, restChar);
		}
	}
}

void SFilterTree::_checkMark(SFilterTreeNode* parent, std::string& keyword)
{
	if (!parent || keyword.size() == 0) return;

	int icount = 0;
	std::string tmpstr(keyword);
	std::string firstChar("");
	std::string endChar("");
	SFilterTreeNode* firstNode = nullptr;
	SFilterTreeNode* endNode = parent;
	while (icount < keyword.size())
	{
		tmpstr = keyword.substr(icount, keyword.size());
		int iBytes = StringFilter::getFirstBytes(tmpstr);
		iBytes = MIN(iBytes, tmpstr.size());
		endChar = tmpstr.substr(0, iBytes);
		endNode = endNode->findChild(endChar);
		if (icount == 0)
		{
			firstChar = endChar;
			firstNode = endNode;
		}
		
		icount += iBytes;
	}

	if (firstNode)
	{
		// 标记为起始位
		firstNode->setBeginMark(true);

		// 判断是否是独立存在词
		int iBytes = StringFilter::getFirstBytes(keyword);
		iBytes = MIN(iBytes, keyword.size());
		firstNode->setStandAlone(iBytes == keyword.size());
	}

	// 标记结束位
	if (endNode)
	{
		endNode->setEndMark(true);
	}
}

std::string SFilterTree::_find(SFilterTreeNode* parent, std::string& keyword)
{
	if (!parent || keyword.empty()) return m_strOut;

	int iBytes = StringFilter::getFirstBytes(keyword);
	std::string firstChar = keyword.substr(0, iBytes);
	SFilterTreeNode* firstNode = parent->findChild(firstChar);

	// 未找到
	if (!firstNode) return m_strOut;

	// 匹配到,先加入缓冲
	m_strBuff.append(firstChar);

	// 对应词组结束
	if (firstNode->getNodesMap().empty())
	{
		m_strOut = m_strBuff;
	}
	// 词组未结束,但存在单个词
	else if (firstNode->hasStandAlone())
	{
		m_strOut = m_strBuff;
	}
	// 词组未结束,但该词有结尾标记
	else if (firstNode->hasEndMark())
	{
		m_strOut = m_strBuff;
	}

	// 继续递归
	std::string restChar = keyword.substr(iBytes, keyword.size());
	return _find(firstNode, restChar);
}

// ----------------------------------------------------------------------------------------------------------------------------------
// SFilterTree

StringFilter::StringFilter()
	: m_bInitialized(false)
{ }

StringFilter::~StringFilter()
{ }

void StringFilter::loadFile(const std::string fileName, char pattern)
{
	std::string pathKey = cocos2d::FileUtils::getInstance()->fullPathForFilename("text/" + fileName);
	unsigned char* pbyBuffer = NULL;
	ssize_t ulSize = 0;
	pbyBuffer = cocos2d::FileUtils::getInstance()->getFileData(pathKey.c_str(), "r", &ulSize);
	if (ulSize)
	{
		SEChar8* pcMem = (SEChar8*)SEMalloc(ulSize + 1);
		memset(pcMem, 0, ulSize + 1);
		SEMemcpy(pcMem, pbyBuffer, ulSize);
		std::string strSensitiveWords = pcMem;
		SEFree(pcMem);

		// 加载过滤词库
		do
		{
			int ipos1 = 0;
			int ipos2 = 0;
			std::string strTmp("");
			for (int i = 0; i < strSensitiveWords.size(); ++i)
			{
				if (strSensitiveWords[i] == pattern)
				{
					ipos2 = i;
					strTmp = strSensitiveWords.substr(ipos1, ipos2 - ipos1).c_str();
					m_tTree.insert(strTmp);
					ipos1 = ipos2 + 1;
				}
			}
		} while (0);
	}
	if (pbyBuffer) delete[] pbyBuffer;

	m_bInitialized = m_tTree.getRootNode().getNodesMap().size() > 0;
}

void StringFilter::loadFile(const std::string fileName, std::string pattern)
{
	std::string pathKey = cocos2d::FileUtils::getInstance()->fullPathForFilename("text/" + fileName);
	unsigned char* pbyBuffer = NULL;
	ssize_t ulSize = 0;
	pbyBuffer = cocos2d::FileUtils::getInstance()->getFileData(pathKey.c_str(), "r", &ulSize);
	if (ulSize)
	{
		SEChar8* pcMem = (SEChar8*)SEMalloc(ulSize + 1);
		memset(pcMem, 0, ulSize + 1);
		SEMemcpy(pcMem, pbyBuffer, ulSize);
		std::string strSensitiveWords = pcMem;
		SEFree(pcMem);

		// 加载过滤词库
		do
		{
			StringFilter::_ArrayStr vStrArray = StringFilter::split(strSensitiveWords, pattern);
			for (int i = 0; i < vStrArray.size(); ++i)
			{
				m_tTree.insert(vStrArray[i]);
			}
		} while (0);
	}
	if (pbyBuffer) delete[] pbyBuffer;

	m_bInitialized = m_tTree.getRootNode().getNodesMap().size() > 0;
}

void StringFilter::loadFileAsync(const std::string fileName, std::string pattern, const LoadFileCallBack& cb)
{
	// 创建线程
	m_thread = std::thread(&StringFilter::_onLoadFileAsync, this, fileName, pattern, cb);

	// 非阻塞
	m_thread.detach();
}

void StringFilter::_onLoadFileAsync(const std::string fileName, std::string pattern, const LoadFileCallBack& cb)
{
	long lTimeStart = cocos2d::utils::getTimeInMilliseconds();
	cocos2d::log("StringFilter Thread start...");

	std::mutex mt;
	mt.lock();
	
	// 开始加载词库
	if (pattern.size() == 1)
	{
		this->loadFile(fileName, pattern[0]);
	}
	else
	{
		this->loadFile(fileName, pattern);
	}

	// 注意: 当前线程是子线程,这里调用cocos2d api转到主线程中处理回调
	cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread(cb);

	mt.unlock();
	
	long lTimeEnd = cocos2d::utils::getTimeInMilliseconds();
	cocos2d::log("StringFilter Thread end...[%.2f s]", (float)(lTimeEnd - lTimeStart) / 1000);
}

bool StringFilter::filterWords(std::string& source, bool replace)
{
	// 没有载入过滤词表
	if (!m_bInitialized) return false;

	// 已载入,开始查找
	return m_tTree.find(source, replace);
}

int StringFilter::getFirstBytes(const std::string& str)
{
	if (str.empty()) return 0;

	for (int i = 0; i < (int)str.size(); ++i)
	{
		unsigned char chr = (unsigned char)str.at(i);
		// 如果是该字节是 0XXX XXXX 样式，说明其是一个英文文字，占1字节
		if ((chr >> 7) == 0)
		{
			return 1;
		}
		// 如果该字节是 1111 110X 样式，说明其是一个文字的头，且该文字占6字节
		else if ((chr >> 1) == 126)
		{
			return 6;
		}
		// 如果该字节是 1111 10XX 样式，说明其是一个文字的头，且该文字占5字节
		else if ((chr >> 2) == 62)
		{
			return 5;
		}
		// 如果该字节是 1111 0XXX 样式，说明其是一个文字的头，且该文字占4字节
		else if ((chr >> 3) == 30)
		{
			return 4;
		}
		// 如果该字节是 1110 XXXX 样式，说明其是一个文字的头，且该文字占3字节
		else if ((chr >> 4) == 14)
		{
			return 3;
		}
		// 如果该字节是 110X XXXX 样式，说明其是一个文字的头，且该文字占2字节
		else if ((chr >> 5) == 6)
		{
			return 2;
		}
		else
		{
			continue;
		}
	}

	return 1;
}

int StringFilter::getChartCount(const std::string& str)
{
	int iflag = 0;
	int icount = 0;
	std::string strTmp = str;
	while (iflag < str.size())
	{
		++icount;

		int iBytes = StringFilter::getFirstBytes(strTmp);
		iBytes = MIN(iBytes, strTmp.size());
		strTmp = strTmp.substr(iBytes, strTmp.size());
		iflag += iBytes;
	}
	return icount;
}

StringFilter::_ArrayStr StringFilter::split(const std::string& str, std::string pattern)
{
	StringFilter::_ArrayStr result;
	int pattern_size = pattern.size();
	int lastpos = 0, index = -1;
	while (-1 != (index = str.find(pattern, lastpos)))
	{
		result.push_back(str.substr(lastpos, index - lastpos));
		lastpos = index + pattern_size;
	}

	// 截取最后一个分隔符后的内容
	std::string laststr = str.substr(lastpos);

	// 如果最后一个分隔符后还有内容就push
	if (!laststr.empty())
	{
		result.push_back(laststr);
	}

	return result;
}