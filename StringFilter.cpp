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
	m_vMap.clear();						// ֻ�����Ԫ��,�ڴ沢û�б仯
	//_TreeMap(m_vMap).swap(m_vMap);	// shrink to fit ����������
	_TreeMap().swap(m_vMap);			// �������С�������������������Ϊ�������ɵı���
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
	// ���� map/unordered_map ���в���
	_TreeMapIterator SFilterTreeMapIt = m_vMap.find(nextCharacter);
	return (SFilterTreeMapIt == m_vMap.end()) ? nullptr : &SFilterTreeMapIt->second;
}

SFilterTreeNode* SFilterTreeNode::insertChild(std::string& nextCharacter)
{
	// ��ӽڵ㣬�����ؽڵ�λ��
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
		// ��ǰ��·��
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

	// �Ƿ��滻
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

	// ������,��ջ���
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
		// ���Ϊ��ʼλ
		firstNode->setBeginMark(true);

		// �ж��Ƿ��Ƕ������ڴ�
		int iBytes = StringFilter::getFirstBytes(keyword);
		iBytes = MIN(iBytes, keyword.size());
		firstNode->setStandAlone(iBytes == keyword.size());
	}

	// ��ǽ���λ
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

	// δ�ҵ�
	if (!firstNode) return m_strOut;

	// ƥ�䵽,�ȼ��뻺��
	m_strBuff.append(firstChar);

	// ��Ӧ�������
	if (firstNode->getNodesMap().empty())
	{
		m_strOut = m_strBuff;
	}
	// ����δ����,�����ڵ�����
	else if (firstNode->hasStandAlone())
	{
		m_strOut = m_strBuff;
	}
	// ����δ����,���ô��н�β���
	else if (firstNode->hasEndMark())
	{
		m_strOut = m_strBuff;
	}

	// �����ݹ�
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

		// ���ع��˴ʿ�
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

		// ���ع��˴ʿ�
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
	// �����߳�
	m_thread = std::thread(&StringFilter::_onLoadFileAsync, this, fileName, pattern, cb);

	// ������
	m_thread.detach();
}

void StringFilter::_onLoadFileAsync(const std::string fileName, std::string pattern, const LoadFileCallBack& cb)
{
	long lTimeStart = cocos2d::utils::getTimeInMilliseconds();
	cocos2d::log("StringFilter Thread start...");

	std::mutex mt;
	mt.lock();
	
	// ��ʼ���شʿ�
	if (pattern.size() == 1)
	{
		this->loadFile(fileName, pattern[0]);
	}
	else
	{
		this->loadFile(fileName, pattern);
	}

	// ע��: ��ǰ�߳������߳�,�������cocos2d apiת�����߳��д���ص�
	cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread(cb);

	mt.unlock();
	
	long lTimeEnd = cocos2d::utils::getTimeInMilliseconds();
	cocos2d::log("StringFilter Thread end...[%.2f s]", (float)(lTimeEnd - lTimeStart) / 1000);
}

bool StringFilter::filterWords(std::string& source, bool replace)
{
	// û��������˴ʱ�
	if (!m_bInitialized) return false;

	// ������,��ʼ����
	return m_tTree.find(source, replace);
}

int StringFilter::getFirstBytes(const std::string& str)
{
	if (str.empty()) return 0;

	for (int i = 0; i < (int)str.size(); ++i)
	{
		unsigned char chr = (unsigned char)str.at(i);
		// ����Ǹ��ֽ��� 0XXX XXXX ��ʽ��˵������һ��Ӣ�����֣�ռ1�ֽ�
		if ((chr >> 7) == 0)
		{
			return 1;
		}
		// ������ֽ��� 1111 110X ��ʽ��˵������һ�����ֵ�ͷ���Ҹ�����ռ6�ֽ�
		else if ((chr >> 1) == 126)
		{
			return 6;
		}
		// ������ֽ��� 1111 10XX ��ʽ��˵������һ�����ֵ�ͷ���Ҹ�����ռ5�ֽ�
		else if ((chr >> 2) == 62)
		{
			return 5;
		}
		// ������ֽ��� 1111 0XXX ��ʽ��˵������һ�����ֵ�ͷ���Ҹ�����ռ4�ֽ�
		else if ((chr >> 3) == 30)
		{
			return 4;
		}
		// ������ֽ��� 1110 XXXX ��ʽ��˵������һ�����ֵ�ͷ���Ҹ�����ռ3�ֽ�
		else if ((chr >> 4) == 14)
		{
			return 3;
		}
		// ������ֽ��� 110X XXXX ��ʽ��˵������һ�����ֵ�ͷ���Ҹ�����ռ2�ֽ�
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

	// ��ȡ���һ���ָ����������
	std::string laststr = str.substr(lastpos);

	// ������һ���ָ����������ݾ�push
	if (!laststr.empty())
	{
		result.push_back(laststr);
	}

	return result;
}