/*********************************************************************************************************************
**********************************************************************************************************************
**********************************************************************************************************************
Author:		Aries
Date:		2018/05/09
Class:		字符过滤类
Detail:		基于 DFA (Deterministic Finite Automaton 确定有穷自动机)算法实现高效敏感字符过滤
**********************************************************************************************************************
**********************************************************************************************************************
**********************************************************************************************************************/

#ifndef __STRINGFILTER_H__
#define __STRINGFILTER_H__

#include "SEEngine.h"

struct stuPosMark
{
	int ibegin = 0;
	int iend = 0;
};

class SFilterTree;
class SFilterTreeNode
{
	typedef std::map<std::string, SFilterTreeNode> _TreeMap;
	typedef std::map<std::string, SFilterTreeNode>::iterator _TreeMapIterator;
	// typedef unordered_map<string, TreeNode> _TreeMap;
	// typedef unordered_map<string, TreeNode>::iterator _TreeMapIterator;

public:
	SFilterTreeNode();
	SFilterTreeNode(std::string character);
	~SFilterTreeNode();

	void reset();

	void setStandAlone(bool b){ m_bStandAlone = b; }
	bool hasStandAlone(){ return m_bStandAlone; }

	void setBeginMark(bool b){ m_bBeginMark = b; }
	bool hasBeginMark(){ return m_bBeginMark; }

	void setEndMark(bool b){ m_bEndMark = b; }
	bool hasEndMark(){ return m_bEndMark; }

	const _TreeMap& getNodesMap() { return m_vMap; }
	std::string getCharacter() const { return m_sCharacter; }
	SFilterTreeNode* findChild(std::string& nextCharacter);
	SFilterTreeNode* insertChild(std::string& nextCharacter);

private:
	_TreeMap			m_vMap;												// 该节点分支符表
	SFilterTreeNode*	m_pParent;											// 所关联的父节点
	std::string			m_sCharacter;										// 该节点首字符
	bool				m_bStandAlone;										// 是否有单独存在的字符
	bool				m_bBeginMark;										// 该节点字符是否可标记为起始位
	bool				m_bEndMark;											// 该节点字符是否可标记为结束位
};

class SFilterTree
{
public:
	SFilterTree();
	~SFilterTree();

	void resetBuffer();
	void insert(std::string& keyword);
	void insert(const char* keyword);
	bool find(std::string& keyword, bool replay);
	SFilterTreeNode& getRootNode(){ return m_nodeRoot; }

private:
	void _insert(SFilterTreeNode* parent, std::string& keyword);			// 递归插入树节点
	void _insertBranch(SFilterTreeNode* parent, std::string& keyword);		// 递归插入关联节点
	void _checkMark(SFilterTreeNode* parent, std::string& keyword);			// 插入树节点后标记
	std::string _find(SFilterTreeNode* parent, std::string& keyword);		// 递归查找(返回当前树路径上的目标串)

private:
	int					m_bMatch;											// 当前匹配结果
	std::string			m_strOut;											// 当前过滤输出
	std::string			m_strBuff;											// 当前过滤缓冲
	SFilterTreeNode		m_nodeRoot;											// 当前树表根节点
};

class StringFilter : public SESingleton < StringFilter >
{
public:
	typedef std::vector<std::string> _ArrayStr;
	typedef std::function<void()> LoadFileCallBack;

public:
	StringFilter();
	~StringFilter();

	void loadFile(const std::string fileName, char pattern);											// 加载文件(单个字符分割,效率高)
	void loadFile(const std::string fileName, std::string pattern);										// 加载文件(整个字符串分割,效率低)
	void loadFileAsync(const std::string fileName, std::string pattern, const LoadFileCallBack& cb);	// 异步加载
	bool IsInitialized(){ return m_bInitialized; }														// 是否初始化
	bool filterWords(std::string& source, bool replace);												// 开始过滤
	static int getFirstBytes(const std::string& str);													// 获取字符串中的第一个字符字节长度
	static int getChartCount(const std::string& str);													// 获取字符串字符个数(有可能一个字符占多个字节)
	static _ArrayStr split(const std::string& str, std::string pattern);								// 字符串分割(支持多字符 如",,")

private:
	void _onLoadFileAsync(const std::string fileName, std::string pattern, const LoadFileCallBack& cb);	// 异步加载线程

private:
	SFilterTree			m_tTree;																		// 过滤词树
	bool				m_bInitialized;																	// 是否初始化
	std::thread			m_thread;																		// 加载线程
};

#define g_pStringFilter StringFilter::GetSingletonPtr()

#endif //__STRINGFILTER_H__