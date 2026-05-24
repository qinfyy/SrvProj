

class Activity
{
public:
	Activity() = default;
	~Activity() = default;

	void Init();

	PlayerInfo ToProto() const;

	PlayerCompBin mPlayerBin;


};