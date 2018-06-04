/*********************************************************************************************************************
**********************************************************************************************************************
**********************************************************************************************************************
Author:		Aries
Date:		2018/05/09
Class:		�ַ�������
Detail:		���� DFA (Deterministic Finite Automaton ȷ�������Զ���)�㷨ʵ�ָ�Ч�����ַ�����
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
	_TreeMap			m_vMap;												// �ýڵ��֧����
	SFilterTreeNode*	m_pParent;											// �������ĸ��ڵ�
	std::string			m_sCharacter;										// �ýڵ����ַ�
	bool				m_bStandAlone;										// �Ƿ��е������ڵ��ַ�
	bool				m_bBeginMark;										// �ýڵ��ַ��Ƿ�ɱ��Ϊ��ʼλ
	bool				m_bEndMark;											// �ýڵ��ַ��Ƿ�ɱ��Ϊ����λ
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
	void _insert(SFilterTreeNode* parent, std::string& keyword);			// �ݹ�������ڵ�
	void _insertBranch(SFilterTreeNode* parent, std::string& keyword);		// �ݹ��������ڵ�
	void _checkMark(SFilterTreeNode* parent, std::string& keyword);			// �������ڵ����
	std::string _find(SFilterTreeNode* parent, std::string& keyword);		// �ݹ����(���ص�ǰ��·���ϵ�Ŀ�괮)

private:
	int					m_bMatch;											// ��ǰƥ����
	std::string			m_strOut;											// ��ǰ�������
	std::string			m_strBuff;											// ��ǰ���˻���
	SFilterTreeNode		m_nodeRoot;											// ��ǰ������ڵ�
};

class StringFilter : public SESingleton < StringFilter >
{
public:
	typedef std::vector<std::string> _ArrayStr;
	typedef std::function<void()> LoadFileCallBack;

public:
	StringFilter();
	~StringFilter();

	void loadFile(const std::string fileName, char pattern);											// �����ļ�(�����ַ��ָ�,Ч�ʸ�)
	void loadFile(const std::string fileName, std::string pattern);										// �����ļ�(�����ַ����ָ�,Ч�ʵ�)
	void loadFileAsync(const std::string fileName, std::string pattern, const LoadFileCallBack& cb);	// �첽����
	bool IsInitialized(){ return m_bInitialized; }														// �Ƿ��ʼ��
	bool filterWords(std::string& source, bool replace);												// ��ʼ����
	static int getFirstBytes(const std::string& str);													// ��ȡ�ַ����еĵ�һ���ַ��ֽڳ���
	static int getChartCount(const std::string& str);													// ��ȡ�ַ����ַ�����(�п���һ���ַ�ռ����ֽ�)
	static _ArrayStr split(const std::string& str, std::string pattern);								// �ַ����ָ�(֧�ֶ��ַ� ��",,")

private:
	void _onLoadFileAsync(const std::string fileName, std::string pattern, const LoadFileCallBack& cb);	// �첽�����߳�

private:
	SFilterTree			m_tTree;																		// ���˴���
	bool				m_bInitialized;																	// �Ƿ��ʼ��
	std::thread			m_thread;																		// �����߳�
};

#define g_pStringFilter StringFilter::GetSingletonPtr()

#endif //__STRINGFILTER_H__