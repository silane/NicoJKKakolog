struct JKID_NAME_ELEM {
	int jkID;
	LPCTSTR name;
	struct COMPARE {
		bool operator()(const JKID_NAME_ELEM &l, const JKID_NAME_ELEM &r) { return l.jkID < r.jkID; }
	};
};

static const JKID_NAME_ELEM DEFAULT_JKID_NAME_TABLE[] = {
	// �j�R�j�R�����`�����l�����X�g(2013-01-02���_)(jkID�Ń\�[�g�K�{)
	{   1, TEXT("NHK ����") },
	{   2, TEXT("E�e��") },
	{   4, TEXT("���{�e���r") },
	{   5, TEXT("�e���r����") },
	{   6, TEXT("TBS �e���r") },
	{   7, TEXT("�e���r����") },
	{   8, TEXT("�t�W�e���r") },
	{   9, TEXT("TOKYO MX") },
	{  10, TEXT("�e����") },
	{  11, TEXT("tvk") },
	{  12, TEXT("�`�o�e���r") },
	{ 101, TEXT("NHKBS-1") },
	{ 103, TEXT("NHK BS�v���~�A��") },
	{ 141, TEXT("BS ���e��") },
	{ 151, TEXT("BS ����") },
	{ 161, TEXT("BS-TBS") },
	{ 171, TEXT("BS�W���p��") },
	{ 181, TEXT("BS�t�W") },
	{ 191, TEXT("WOWOW�v���C��") },
	{ 192, TEXT("WOWOW���C�u") },
	{ 193, TEXT("WOWOW�V�l�}") },
	{ 200, TEXT("�X�^�[�`�����l��1") },
	{ 201, TEXT("�X�^�[�`�����l��2") },
	{ 202, TEXT("�X�^�[�`�����l��3") },
	{ 211, TEXT("BS�C���u��") },
	{ 222, TEXT("TwellV") },
	{ 231, TEXT("������w") },
	{ 234, TEXT("BS�O���[���`�����l��") },
	{ 236, TEXT("BS�A�j�}�b�N�X") },
	{ 238, TEXT("FOX bs 238") },
	{ 241, TEXT("BS�X�J�p�[!") },
	{ 242, TEXT("J Sports 1") },
	{ 243, TEXT("J Sports 2") },
	{ 244, TEXT("J Sports 3") },
	{ 245, TEXT("J Sports 4") },
	{ 251, TEXT("BS�ނ�r�W����") },
	{ 252, TEXT("IMAGICA BS") },
	{ 255, TEXT("BS���{�f����`�����l��") },
	{ 256, TEXT("�f�B�Y�j�[�E�`�����l��") },
	{ 258, TEXT("Dlife") },
	{ 910, TEXT("SOLiVE24") },
};
